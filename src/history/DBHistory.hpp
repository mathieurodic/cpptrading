#ifndef CTRADING__HISTORY__DBHISTORY__HPP
#define CTRADING__HISTORY__DBHISTORY__HPP


#include <string>
#include <deque>
#include <set>
#include <chrono>
#include <thread>
#include <cmath>

#include "db/PlainLog.hpp"
#include "db/UpscaleBTree.hpp"

#include "IO/directories.hpp"

#include "./History.hpp"


class DBHistory : public History{
public:

    inline DBHistory(const std::string& basepath) :
        _basepath(basepath),
        _basepath_is_initialized(init_directory(_basepath)),
        _balance_changes(basepath + "/balance_changes"),
        _balance_changes_by_timestamp(basepath + "/balance_changes_by_timestamp"),
        _trades(basepath + "/trades"),
        _trades_by_timestamp(basepath + "/trades_by_timestamp"),
        _orders(basepath + "/orders"),
        _decisions(basepath + "/decisions"),
        _decisions_by_timestamp(basepath + "/decisions_by_timestamp")
    {}

    inline bool init_directory(const std::string& path) {
        try {
            make_directory(path);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

    virtual void feed(BalanceChange& balance_change) {
        _balance_changes.append(balance_change);
        _balance_changes_by_timestamp.insert(balance_change.timestamp, balance_change);
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
        _decisions_by_timestamp.insert(decision.timestamp, decision);
    }

    virtual Range<Trade> get_trades_by_timestamp(Timestamp timestamp_begin, Timestamp timestamp_end) {
        return _trades_by_timestamp.get(timestamp_begin, timestamp_end);
    }
    virtual const Balance get_balance_at_timestamp(Timestamp& timestamp) {
        Balance result;
        for (const BalanceChange& balance_change : get_balance_changes()) {

        }
        return result;
    }
    virtual Range<BalanceChange> get_balance_changes() {
        return _balance_changes.get<BalanceChange>();
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

    virtual TimestampSpan get_time_span() {
        TimestampSpan span;
        try {
            span.from = _trades_by_timestamp.get_lowest_key();
        } catch (const UpscaleDBException& exception) {
            span.from = NAN;
        }
        try {
            span.to = _trades_by_timestamp.get_highest_key();
        } catch (const UpscaleDBException& exception) {
            span.to = NAN;
        }
        return span;
    }

protected:

    const std::string _basepath;

private:

    bool _basepath_is_initialized;

    PlainLogWriter _balance_changes;
    UpscaleBTree<Timestamp, BalanceChange> _balance_changes_by_timestamp;

    PlainLogWriter _trades;
    UpscaleBTree<Timestamp, Trade> _trades_by_timestamp;

    PlainLogWriter _orders;

    PlainLogWriter _decisions;
    UpscaleBTree<Timestamp, Decision> _decisions_by_timestamp;

};


#endif // CTRADING__HISTORY__DBHISTORY__HPP
