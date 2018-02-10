#ifndef CTRADING__HISTORY__LOGHISTORY__HPP
#define CTRADING__HISTORY__LOGHISTORY__HPP


#include "./History.hpp"

#include <string>
#include <iostream>
#include <fstream>


template <typename Model>
class LogHistoryIterator : public Iterator<Model> {
public:

    inline LogHistoryIterator(const LogHistoryIterator<Model>& source) :
        _path(source._path),
        _file(_path),
        _position(source._position)
    {
        this->_is_finished = source._is_finished;
        _file.seekg(_position);
        this->_value = source._value;
    }
    inline LogHistoryIterator() {
        this->_is_finished = true;
    }
    inline LogHistoryIterator(const std::string& path) :
        _path(path),
        _file(_path)
    {
        this->_is_finished = false;
        next();
    }

    virtual void operator++() {
        next();
    }

private:
    
    inline bool next() {
        std::string line;
        do {
            this->_is_finished = !std::getline(_file, line);
            _position = _file.tellg();
        } while (!this->_is_finished && !this->_value.parse(line));
    }

    static const std::string _model_name;
    const std::string _path;
    std::ifstream _file;
    size_t _position;
};

template <> const std::string LogHistoryIterator<Trade>::_model_name = "Trade";
template <> const std::string LogHistoryIterator<Order>::_model_name = "Order";
template <> const std::string LogHistoryIterator<Decision>::_model_name = "Decision";


template <typename Model>
class LogHistoryRange : public Range<Model, LogHistoryIterator<Model>> {
public:

    inline LogHistoryRange() :
        _is_stdout(true)
    {}
    inline LogHistoryRange(const std::string& path) :
        _path(path),
        _is_stdout(false)
    {}

    inline LogHistoryIterator<Model> begin() {
        return _is_stdout ? LogHistoryIterator<Model>() : LogHistoryIterator<Model>(_path);
    }

private:
     const std::string _path;
     const bool _is_stdout;
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

    template <typename Model>
    LogHistoryRange<Model> get_all() {
        return _is_stdout ? LogHistoryRange<Model>() : LogHistoryRange<Model>(_path);
    }

private:

    template <typename Model>
    inline void _feed(Model& instance) {
        (get_output() << instance << '\n').flush();
    }

    const std::string _path;
    const bool _is_stdout;
    std::ofstream _file;
};


#endif // CTRADING__HISTORY__LOGHISTORY__HPP
