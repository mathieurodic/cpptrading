#ifndef CTRADING__DB__PLAINLOG__HPP
#define CTRADING__DB__PLAINLOG__HPP


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <experimental/filesystem>
#include <set>

#include "exceptions/Exception.hpp"
#include "range/Range.hpp"


class PlainLogReader {
public:

    inline PlainLogReader(const std::string& basepath) :
        _path(basepath),
        _file(NULL)
    {
        _file = fopen(basepath.c_str(), "rb");
    }

    inline ~PlainLogReader() {
        if (_file != NULL) {
            fclose(_file);
            _file = NULL;
        }
    }

    template <typename item_t>
    inline const bool next(item_t& item) {
        if (_file == NULL) {
            return false;
        }
        int result = fread(&item, sizeof(item), 1, _file);
        switch (result) {
            case 1:
                return true;
            case 0:
                return false;
            default:
                throw FileException("PlainLogReader could not read from file", result, strerror(errno));
        }
    }

private:
    const std::string _path;
    FILE* _file;

};


template <typename T>
class PlainLogRangeData : public RangeData<T> {
public:

    PlainLogRangeData(const std::string& basepath) :
        _reader(basepath)
        {

    }

    virtual const bool init(T*& value) {
        value = & _value;
        return _reader.next(_value);
    }
    virtual const bool next(T*& value) {
        return _reader.next(_value);
    }

private:

    PlainLogReader _reader;
    T _value;

};

template <typename T>
class PlainLogRange : public Range<T> {
public:

    PlainLogRange(const std::string& basepath) :
        Range<T>(new PlainLogRangeData<T>(basepath)) {}

};


class PlainLogWriter {
public:

    inline PlainLogWriter(const std::string& basepath, const int64_t& interval=86400, const size_t& check_threshold=256) :
        _path(basepath),
        _file(fopen(_path.c_str(), "ab"))
    {
        if (_file == NULL) {
            throw FileException("PlainLogWriter could not open file for writing", _path, strerror(errno));
        }
    }

    inline ~PlainLogWriter() {
        if (_file != NULL) {
            fclose(_file);
            _file = NULL;
        }
    }

    template <typename item_t>
    inline void append(const item_t& item) {
        if (fwrite(&item, sizeof(item), 1, _file) != 1) {
            throw FileException("PlainLogWriter could not write to file", _path, strerror(errno));
        }
        if (fflush(_file) != 0) {
            throw FileException("PlainLogWriter could not flush to file", _path, strerror(errno));
        }
    }

    template <typename T>
    inline PlainLogRange<T> get() {
        return PlainLogRange<T>(_path);
    }

private:
    const std::string _path;
    FILE* _file;

};


struct PlainLog {
    typedef PlainLogWriter Writer;
    typedef PlainLogReader Reader;

    template <typename T>
    using Range = PlainLogRange<T>;
};


#endif // CTRADING__DB__PLAINLOG__HPP
