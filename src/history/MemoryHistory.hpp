#ifndef CTRADING__HISTORY__MEMORYHISTORY__HPP
#define CTRADING__HISTORY__MEMORYHISTORY__HPP


#include <map>
#include <deque>


template <typename T>
class DequeRangeData : public RangeData<T> {
public:

    inline DequeRangeData(std::deque<T>& data) :
        _data(data) {}

    virtual const bool init(T& value) {
        _data_iterator = _data.begin();
        if (_data_iterator != _data.end()) {
            value = *_data_iterator;
            return true;
        }
        return false;
    }

    virtual const bool next(T& value) {
        ++_data_iterator;
        if (_data_iterator != _data.end()) {
            value = *_data_iterator;
            return true;
        }
        return false;
    }


private:

    std::deque<T>& _data;
    typename std::deque<T>::iterator _data_iterator;

};

template <typename T>
class DequeRange : public Range<T> {
public:

    inline DequeRange(std::deque<T>& data) :
        Range<T>(new DequeRangeData<T>(data)) {}

};


class MemoryHistory : public History {
public:

    virtual void feed(Trade& trade) {
        _trades_by_id.insert({trade.id, trade});
        _trades_by_timestamp.insert({trade.timestamp, trade});
        _trades.push_back(trade);
    }
    virtual void feed(Order& order) {
        _orders_by_id.insert({order.id, order});
    }
    virtual void feed(Decision& decision) {
        _decisions_by_timestamp.insert({decision.decision_timestamp, decision});
    }

    virtual TradeAverage get_average(const double& timestamp_begin, const double& timestamp_end) {
        TradeAverage result;
        auto it_lower = _trades_by_timestamp.lower_bound(timestamp_begin);
        auto it_upper = _trades_by_timestamp.upper_bound(timestamp_begin);
        for (auto it=it_lower; it!=it_upper; ++it) {
            result += it->second;
        }
        return result;
    }

    virtual Range<Trade> get_trades() {
        return DequeRange<Trade>(_trades);
    }
    virtual Range<Order> get_orders() {
        return DequeRange<Order>(_orders);
    }
    virtual Range<Decision> get_decisions() {
        return DequeRange<Decision>(_decisions);
    }

    virtual Span get_time_span() {
        Span span;
        {
            auto it = _trades_by_timestamp.begin();
            if (it != _trades_by_timestamp.end()) {
                span.min = it->first;
            }
        }
        {
            auto it = _trades_by_timestamp.rbegin();
            if (it != _trades_by_timestamp.rend()) {
                span.max = it->first;
            }
        }
        return span;
    }

protected:

    const std::string _basepath;

private:

    std::deque<Trade> _trades;
    std::multimap<uint64_t, Trade> _trades_by_id;
    std::multimap<double, Trade> _trades_by_timestamp;
    std::deque<Order> _orders;
    std::multimap<uint64_t, Order> _orders_by_id;
    std::deque<Decision> _decisions;
    std::multimap<double, Decision> _decisions_by_timestamp;

};


#endif // CTRADING__HISTORY__MEMORYHISTORY__HPP
