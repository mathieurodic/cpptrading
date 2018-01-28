#ifndef CTRADING__DB__GZIPLOG__HPP
#define CTRADING__DB__GZIPLOG__HPP


#include <string>
#include <zlib.h>

#include "../exceptions/Exception.hpp"


class GzipLogWriter {
public:

    inline GzipLogWriter(const std::string& path) :
        _path(path),
        _file(gzopen(path.c_str(), "ab"))
    {
        if (_file == Z_NULL) {
            throw FileException("Error while open file in append-only mode", _path, strerror(errno));
        }
    }
    inline ~GzipLogWriter() {
        if (_file) {
            gzclose(_file);
        }
    }

    inline void check(int result) {
        switch (result) {
            case Z_ERRNO:
                throw FileException("Error while appending to file", _path, strerror(errno));
            case Z_STREAM_ERROR:
                throw FileException("Error while appending to file", _path, "The stream is invalid, is not open for writing, or is in an invalid state.");
            case Z_BUF_ERROR:
                throw FileException("Error while appending to file", _path, "No compression progress is possible");
            case Z_MEM_ERROR:
                throw FileException("Error while appending to file", _path, "Insufficient memory available to compress.");
        }
    }

    template <typename item_t>
    inline void append(const item_t& item) {
        check(gzwrite(_file, &item, sizeof(item)));
        check(gzflush(_file, Z_FINISH));
    }
    inline void append(const char* item) {
        check(gzputs(_file, item));
        check(gzflush(_file, Z_FINISH));
    }
    inline void append(const std::string& item) {
        check(gzwrite(_file, item.data(), item.size()));
        check(gzflush(_file, Z_FINISH));
    }

private:
    const std::string _path;
    gzFile _file;
};


class GzipLogReader {
public:

    inline GzipLogReader(const std::string& path) :
        _path(path),
        _file(gzopen(path.c_str(), "rb"))
    {
        if (_file == Z_NULL) {
            throw FileException("Error while open file in read-only mode", _path, strerror(errno));
        }
    }
    inline ~GzipLogReader() {
        if (_file) {
            gzclose(_file);
        }
    }

    template <typename T>
    inline const T next() {
        T result;
        gzread(_file, &result, sizeof(T));
        return result;
    }
    template <typename T>
    inline int next(T& item) {
        return gzread(_file, &item, sizeof(T));
    }

private:
    const std::string _path;
    gzFile _file;
};


#endif // CTRADING__DB__GZIPLOG__HPP
