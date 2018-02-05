#ifndef CTRADING__HISTORY__HISTORY__HPP
#define CTRADING__HISTORY__HISTORY__HPP


#include <models/Trade.hpp>
#include <models/TradeAverage.hpp>
#include <models/Order.hpp>
#include <models/Decision.hpp>


template <typename Model>
class HistoryIterator {
public:

    HistoryIterator() : _is_finished(true) {}

    virtual const Model& operator*() const {
        return _value;
    }

    virtual void operator++() {}

    virtual const bool operator!=(const HistoryIterator<Model>& other) const {
        return _is_finished != other._is_finished;
    }

private:

    bool _is_finished;
    Model _value;

};


template <typename Model>
class HistoryRange {
public:

    HistoryRange() {}

    virtual HistoryIterator<Model> begin() {
        return HistoryIterator<Model>();
    }
    const HistoryIterator<Model>& end() const {
        return _end;
    }

private:
    static const HistoryIterator<Model> _end;
};


class History {
public:

    virtual void feed(Trade& trade) = 0;
    virtual void feed(Order& order) = 0;
    virtual void feed(Decision& decision) = 0;

    virtual TradeAverage get_average(const double& timestamp_begin, const double timestamp_end) {
        return TradeAverage();
    }

    virtual HistoryRange<Trade> get_trades() {
        return HistoryRange<Trade>();
    }

};


#endif // CTRADING__HISTORY__HISTORY__HPP
