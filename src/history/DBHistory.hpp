#ifndef CTRADING__HISTORY__DBHISTORY__HPP
#define CTRADING__HISTORY__DBHISTORY__HPP


#include <string>
#include <set>
#include <chrono>
#include <thread>
#include <cmath>

#include <tbb/concurrent_hash_map.h>

#include "db/IndexedStorage.hpp"
#include "db/RotatingLog.hpp"
#include "db/UpscaleBTree.hpp"

#include "./History.hpp"


class DBHistory : public History {
public:

    inline DBHistory(const std::string& basepath) :
        _basepath(basepath),
        _trades(basepath + "/trades"),
        _orders(basepath + "/orders"),
        _decisions(basepath + "/decisions")
    {}

    virtual void feed(Trade& trade) {
        _trades.insert(trade);
    }
    virtual void feed(Order& order) {
        _orders.insert(order);
    }
    virtual void feed(Decision& decision) {
        _decisions.insert(decision);
    }

    virtual TradeAverage get_average(const double& timestamp_begin, const double& timestamp_end) {
        TradeAverage result;
        for (const Trade& trade : _trades.get_timestamp_range(timestamp_begin, timestamp_end)) {
            result += trade;
        }
        return result;
    }

protected:

    const std::string _basepath;

private:

    INDEXED_STORAGE_2(Trade, id, timestamp, RotatingLog, UpscaleBTree) _trades;
    INDEXED_STORAGE(Order, id, RotatingLog, UpscaleBTree) _orders;
    INDEXED_STORAGE(Decision, decision_timestamp, RotatingLog, UpscaleBTree) _decisions;

};


#endif // CTRADING__HISTORY__DBHISTORY__HPP
