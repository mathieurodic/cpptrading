#ifndef CTRADING__HISTORY__MEMORYHISTORY__HPP
#define CTRADING__HISTORY__MEMORYHISTORY__HPP


#include <map>
#include <deque>

#include "range/ForwardRange.hpp"


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
        return ForwardRangeFactory(_trades);
    }
    virtual Range<Order> get_orders() {
        return ForwardRangeFactory(_orders);
    }
    virtual Range<Decision> get_decisions() {
        return ForwardRangeFactory(_decisions);
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
