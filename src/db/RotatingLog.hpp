#ifndef CTRADING__DB__ROTATINGLOG__HPP
#define CTRADING__DB__ROTATINGLOG__HPP


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <experimental/filesystem>
#include <set>

#include "exceptions/Exception.hpp"
#include "iteration.hpp"


class RotatingLogReader {
public:

    inline RotatingLogReader(const std::string& basepath) :
        _basepath(basepath),
        _file(NULL)
    {
        std::experimental::filesystem::path b = _basepath;
        for (auto& p : std::experimental::filesystem::directory_iterator(b.parent_path())) {
            const std::string filepath = p.path().string();
            if (filepath.rfind(_basepath, 0) == 0) {
                _paths.insert(filepath);
                std::cout << "filepath = " << filepath << '\n';
            }
        }
        _paths_it = _paths.begin();
    }

    inline ~RotatingLogReader() {
        close_file();
    }

    inline bool close_file() {
        if (_file != NULL) {
            fclose(_file);
            _file = NULL;
        }
    }
    inline bool rotate_file() {
        if (_file == NULL) {
            if (_paths_it == _paths.end()) {
                return false;
            }
            _file = fopen((_paths_it++)->c_str(), "rb");
            if (_file == NULL) {
                throw FileException("RotatingLogReader could not open file for reading", *_paths_it, strerror(errno));
            }
        }
        return true;
    }

    template <typename item_t>
    inline const bool next(item_t& item) {
        if (!rotate_file()) {
            return false;
        }
        int read_result;
        do {
            read_result = fread(&item, sizeof(item), 1, _file);
            switch (read_result) {
                case 1:
                    return true;
                case 0:
                    close_file();
                    if (!rotate_file()) {
                        return false;
                    }
                    break;
                default:
                    throw FileException("RotatingLogReader could not read from file", *_paths_it, read_result, strerror(errno));
            }
        } while (read_result == 0);
        return false;
    }

private:
    const std::string _basepath;
    std::set<std::string> _paths;
    std::set<std::string>::iterator _paths_it;
    FILE* _file;

};


template <typename T>
class RotatingLogIterator : public Iterator<T> {
public:

    inline RotatingLogIterator() {
        this->_is_finished = true;
        _reader = NULL;
    }
    inline RotatingLogIterator(const std::string basepath) {
        this->_is_finished = false;
        _reader = new RotatingLogReader(basepath);
        next();
    }
    inline ~RotatingLogIterator() {
        if (_reader != NULL) {
            delete _reader;
            _reader = NULL;
        }
    }

    virtual void operator++() {
        next();
    }

private:

    inline void next() {
        if (!this->_is_finished) {
            this->_is_finished = ! _reader->next(this->_value);
        }
    }

    RotatingLogReader* _reader;

};


template <typename T>
class RotatingLogRange : public Range<T, RotatingLogIterator<T>> {
public:

    inline RotatingLogRange(const std::string& basepath) : _basepath(basepath) {
        std::cout << "basepath = " << basepath << '\n';
    }

    RotatingLogIterator<T> begin() {
        std::cout << "BEGIN " << _basepath << '\n';
        return RotatingLogIterator<T>(_basepath);
    }

private:

    const std::string _basepath;
    T _value;

};


class RotatingLogWriter {
public:

    inline RotatingLogWriter(const std::string& basepath, const int64_t& interval=86400, const size_t& check_threshold=256) :
        _basepath(basepath),
        _interval(interval),
        _check_threshold(check_threshold),
        _bytes_written(0),
        _file(NULL)
    {
        start();
    }

    inline ~RotatingLogWriter() {
        stop();
    }

    inline void start() {
        stop();
        // format date
        _last_timestamp = time(NULL);
        struct tm lt;
        localtime_r(&_last_timestamp, &lt);
        static const char* suffix_format = "%Y-%m-%dT%H:%M:%S";
        char suffix[32];
        memset(suffix, 0, sizeof(suffix));
        strftime(suffix, sizeof(suffix), suffix_format, &lt);
        // build path
        _path = _basepath + '.' + suffix;
        _file = fopen(_path.c_str(), "ab");
        if (_file == NULL) {
            throw FileException("RotatingLogWriter could not open file for writing", _path, strerror(errno));
        }
    }
    inline void stop() {
        if (_file != NULL) {
            fclose(_file);
            _file = NULL;
        }
    }

    template <typename item_t>
    inline void append(const item_t& item) {
        if (fwrite(&item, sizeof(item), 1, _file) != 1) {
            throw FileException("RotatingLogWriter could not write to file", _path, strerror(errno));
        }
        if (fflush(_file) != 0) {
            throw FileException("RotatingLogWriter could not flush to file", _path, strerror(errno));
        }
        _bytes_written += sizeof(item);
        if (_bytes_written > _check_threshold)  {
            const int64_t current_timestamp = time(NULL);
            if (current_timestamp - _last_timestamp >= _interval) {
                start();
                _bytes_written = 0;
            }
        }
    }

    template <typename T>
    inline RotatingLogRange<T> get_all() {
        return RotatingLogRange<T>(_basepath);
    }

private:
    const std::string _basepath;
    int64_t _interval;
    size_t _bytes_written;
    size_t _check_threshold;
    int64_t _last_timestamp;
    std::string _path;
    FILE* _file;

};


struct RotatingLog {
    typedef RotatingLogWriter Writer;
    typedef RotatingLogReader Reader;

    template <typename T>
    using Range = RotatingLogRange<T>;
};


#endif // CTRADING__DB__ROTATINGLOG__HPP
