#ifndef CTRADING__EXCEPTIONS__EXCEPTION__HPP
#define CTRADING__EXCEPTIONS__EXCEPTION__HPP


#include <exception>
#include <sstream>
#include <iostream>


class Exception : std::exception {
public:

    template<typename ...Args>
    inline Exception(Args const&... args) : _what(concatenate(args...)) {
        std::cerr << '\n' << _what << '\n';
    }

    const char* what() const noexcept {
        return _what.c_str();
    }

protected:

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


class DBException : public Exception {
    using Exception::Exception;
};

class DBDuplicateException : public DBException {
    using DBException::DBException;
};
class DBKeyException : public DBException {
    using DBException::DBException;
};


#endif // CTRADING__EXCEPTIONS__EXCEPTION__HPP
