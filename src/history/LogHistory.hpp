#ifndef CTRADING__HISTORY__LOGHISTORY__HPP
#define CTRADING__HISTORY__LOGHISTORY__HPP


#include "./History.hpp"

#include <string>
#include <iostream>
#include <fstream>


class LogHistory : public History {
public:

    LogHistory() :
        _is_stdout(true) {}
    LogHistory(const std::string& path) :
        _path(path),
        _is_stdout(false),
        _file(_path.c_str(), std::ofstream::app)
    {

    }

    inline std::ostream& get_output() {
        return _is_stdout ? std::cout : _file;
    }

    virtual void record(Trade& trade) {
        get_output() << trade << '\n';
    }
    virtual void record(Order& order) {
        get_output() << order << '\n';
    }

private:
    const std::string _path;
    const bool _is_stdout;
    std::ofstream _file;
};


#endif // CTRADING__HISTORY__LOGHISTORY__HPP
