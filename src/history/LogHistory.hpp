#ifndef CTRADING__HISTORY__LOGHISTORY__HPP
#define CTRADING__HISTORY__LOGHISTORY__HPP


#include "./History.hpp"

#include <string>
#include <iostream>
#include <fstream>


template <typename T>
class LogHistoryRangeData : public RangeData<T> {
public:

    inline LogHistoryRangeData(const std::string& path) :
        _file(path) {}


    virtual const bool init(T*& value) {
        return iterate(*value);
    }
    virtual const bool next(T*& value) {
        return iterate(*value);
    }

private:

    inline const bool iterate(T& value) {
        std::string line;
        bool must_continue;
        do {
            must_continue = (bool) std::getline(_file, line);
        } while (must_continue && !value.parse(line));
        return must_continue;
    }

    static const std::string _model_name;
    const std::string _path;
    std::ifstream _file;

};

template <> const std::string LogHistoryRangeData<Trade>::_model_name = "Trade";
template <> const std::string LogHistoryRangeData<Order>::_model_name = "Order";
template <> const std::string LogHistoryRangeData<Decision>::_model_name = "Decision";


template <typename T>
class LogHistoryRange : public Range<T> {
public:

    inline LogHistoryRange(const std::string& path) :
        Range<T>(new LogHistoryRangeData<T>(path)) {}

};


class LogHistory : public History {
public:

    LogHistory() :
        _is_stdout(true) {}
    LogHistory(const std::string& path) :
        _path(path),
        _is_stdout(false),
        _file(_path.c_str(), std::ofstream::app)
    {}

    inline std::ostream& get_output() {
        return _is_stdout ? std::cout : _file;
    }

    virtual void feed(Trade& trade) {
        _feed(trade);
    }
    virtual void feed(Order& order) {
        _feed(order);
    }
    virtual void feed(Decision& decision) {
        _feed(decision);
    }

    virtual Range<Trade> get_trades() {
        return _is_stdout ? Range<Trade>() : LogHistoryRange<Trade>(_path);
    }
    virtual Range<Order> get_orders() {
        return _is_stdout ? Range<Order>() : LogHistoryRange<Order>(_path);
    }
    virtual Range<Decision> get_decisions() {
        return _is_stdout ? Range<Decision>() : LogHistoryRange<Decision>(_path);
    }

private:

    template <typename T>
    inline void _feed(T& instance) {
        (get_output() << instance << '\n').flush();
    }

    const std::string _path;
    const bool _is_stdout;
    std::ofstream _file;
};


#endif // CTRADING__HISTORY__LOGHISTORY__HPP
