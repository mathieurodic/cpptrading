#ifndef CTRADING__HISTORY__HISTORY__HPP
#define CTRADING__HISTORY__HISTORY__HPP


#include <string>
#include <deque>
#include <algorithm>

#include "db/IndexedStorage.hpp"

#include "models/Trade.hpp"
#include "models/Order.hpp"
#include "models/Decision.hpp"
#include "models/TradeAverage.hpp"


class History {
public:

    inline History(const std::string& basepath) :
        _basepath(basepath),
        trades(basepath + "/trades.btree", true, true),
        orders(basepath + "/orders.btree", true),
        decisions(basepath + "/decisions.btree", true),
        averages(basepath + "/averages.btree", true)
    {}

    inline const TradeAverage compute_trades_average(const double& timestamp_begin, const double& timestamp_end) {
        TradeAverage result(timestamp_begin, timestamp_end);
        for (const auto& trade : trades.get_timestamp_range(timestamp_begin, timestamp_end)) {
            result += trade;
        }
        return result;
    }

    INDEXED_STORAGE_2(Trade, id, timestamp) trades;
    INDEXED_STORAGE(Order, id) orders;
    INDEXED_STORAGE(Decision, timestamp) decisions;
    INDEXED_STORAGE(TradeAverage, timestamp_range) averages;

private:
    const std::string _basepath;
};


#endif // CTRADING__HISTORY__HISTORY__HPP
