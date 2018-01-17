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
    {}
    inline ~GzipLogWriter() {
        if (_file) {
            gzclose(_file);
        }
    }

    template <typename item_t>
    inline int append(const item_t& item) {
        return gzwrite(_file, &item, sizeof(item));
    }
    inline int append(const char* item) {
        return gzputs(_file, item);
    }
    inline int append(const std::string& item) {
        return gzwrite(_file, item.data(), item.size());
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
    {}
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
