#ifndef CTRADING__EXCEPTIONS__EXCEPTION__HPP
#define CTRADING__EXCEPTIONS__EXCEPTION__HPP


#include <exception>
#include <sstream>
#include <iostream>


class Exception : std::exception {
public:

    template<typename ...Args>
    inline Exception(Args const&... args) : _what(concatenate(args...)) {
        std::cerr << "\033[2m" << '\n' << _what << "\033[0m" << '\n';
    }

    const char* what() const noexcept {
        return _what.c_str();
    }

protected:

    inline std::string concatenate() {
        return "";
    }
    template<typename T>
    inline std::string concatenate(const T& t) {
        std::stringstream string_stream;
        string_stream << t;
        return string_stream.str();
    }

    template<typename T, typename ... Args>
    inline std::string concatenate(const T& first, Args ... args) {
        return concatenate(first) + ", " + concatenate(args...);
    }

    const std::string _what;
};


class KeyException : public Exception {
    using Exception::Exception;
};


class FileException : public Exception {
    using Exception::Exception;
};

class NetworkException : public Exception {
    using Exception::Exception;
};


class DBException : public Exception {
    using Exception::Exception;
};

class DBDuplicateException : public DBException {
    using DBException::DBException;
};
class DBKeyException : public DBException, public KeyException {
    using DBException::DBException;
};


#endif // CTRADING__EXCEPTIONS__EXCEPTION__HPP
