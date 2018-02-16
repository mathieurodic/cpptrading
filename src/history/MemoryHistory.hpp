#ifndef CTRADING__HISTORY__MEMORYHISTORY__HPP
#define CTRADING__HISTORY__MEMORYHISTORY__HPP


#include <map>
#include <deque>

#include "range/ForwardRange.hpp"
#include "range/SortedRange.hpp"


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
        _decisions.push_back(decision);
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

    virtual Range<Trade> get_trades_by_timestamp(Timestamp timestamp_begin, Timestamp timestamp_end) {
        return SortedRangeFactory(_trades_by_timestamp, timestamp_begin, timestamp_end);
    }
    virtual Range<Decision> get_decisions_by_timestamp(Timestamp timestamp_begin, Timestamp timestamp_end) {
        return SortedRangeFactory(_decisions_by_timestamp, timestamp_begin, timestamp_end);
    }

    virtual TimestampSpan get_time_span() {
        TimestampSpan timestamp_span;
        auto it_from = _trades_by_timestamp.begin();
        if (it_from != _trades_by_timestamp.end()) {
            timestamp_span.from = it_from->first;
        }
        auto it_to = _trades_by_timestamp.rbegin();
        if (it_to != _trades_by_timestamp.rend()) {
            timestamp_span.to = it_to->first;
        }
        return timestamp_span;
    }

protected:

    const std::string _basepath;

private:

    std::deque<Trade> _trades;
    std::multimap<uint64_t, Trade> _trades_by_id;
    std::multimap<Timestamp, Trade> _trades_by_timestamp;
    std::deque<Order> _orders;
    std::multimap<uint64_t, Order> _orders_by_id;
    std::deque<Decision> _decisions;
    std::multimap<Timestamp, Decision> _decisions_by_timestamp;

};


#endif // CTRADING__HISTORY__MEMORYHISTORY__HPP
