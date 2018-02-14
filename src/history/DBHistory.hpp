#ifndef CTRADING__HISTORY__DBHISTORY__HPP
#define CTRADING__HISTORY__DBHISTORY__HPP


#include <string>
#include <deque>
#include <set>
#include <chrono>
#include <thread>
#include <cmath>

#include "db/RotatingLog.hpp"
#include "db/UpscaleBTree.hpp"

#include "IO/directories.hpp"

#include "./History.hpp"


class DBHistory : public History{
public:

    inline DBHistory(const std::string& basepath) :
        _basepath(basepath),
        _basepath_is_initialized(init_directory(_basepath)),
        _trades(basepath + "/trades"),
        _trades_by_timestamp(basepath + "/trades_by_timestamp"),
        _orders(basepath + "/orders"),
        _decisions(basepath + "/decisions")
    {}

    inline bool init_directory(const std::string& path) {
        try {
            make_directory(path);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

    virtual void feed(Trade& trade) {
        _trades.append(trade);
        _trades_by_timestamp.insert(trade.timestamp, trade);
    }
    virtual void feed(Order& order) {
        _orders.append(order);
    }
    virtual void feed(Decision& decision) {
        _decisions.append(decision);
    }

    virtual Range<Trade> get_trades_by_timestamp(Timestamp timestamp_begin, Timestamp timestamp_end) {
        return _trades_by_timestamp.get(timestamp_begin, timestamp_end);
    }

    virtual Range<Trade> get_trades() {
        return _trades.get<Trade>();
    }
    virtual Range<Order> get_orders() {
        return _orders.get<Order>();
    }
    virtual Range<Decision> get_decisions() {
        return _decisions.get<Decision>();
    }

protected:

    const std::string _basepath;

private:

    bool _basepath_is_initialized;
    RotatingLogWriter _trades;
    UpscaleBTree<Timestamp, Trade> _trades_by_timestamp;
    RotatingLogWriter _orders;
    RotatingLogWriter _decisions;

};


#endif // CTRADING__HISTORY__DBHISTORY__HPP
