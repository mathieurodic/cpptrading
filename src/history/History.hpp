#ifndef CTRADING__HISTORY__HISTORY__HPP
#define CTRADING__HISTORY__HISTORY__HPP


#include <string>
#include <set>
#include <chrono>
#include <thread>
#include <cmath>

#include <tbb/concurrent_hash_map.h>

#include "db/IndexedStorage.hpp"
#include "db/RotatingLog.hpp"
#include "db/UpscaleBTree.hpp"

#include "models/Trade.hpp"
#include "models/Order.hpp"
#include "models/Decision.hpp"
#include "models/TradeAverage.hpp"


class History {
public:

    inline History(const std::string& basepath, const std::string& currency_pair) :
        _basepath(basepath),
        _currency_pair(currency_pair),
        _is_running(true),
        _cleanup_period(10.0),
        _cleanup_threshold(60.0),
        _cleanup_thread(std::thread(_cleanup, this)),
        last_timestamp(0.0),
        trades(basepath + "/trades"),
        orders(basepath + "/orders"),
        decisions(basepath + "/decisions"),
        averages(basepath + "/averages")
    {}
    inline ~History() {
        _is_running = false;
        _cleanup_thread.join();
    }

    inline const TradeAverage compute_trades_average(const double& timestamp_begin, const double& timestamp_end) {
        TradeAverage result(timestamp_begin, timestamp_end);
        for (const auto& trade : trades.get_timestamp_range(timestamp_begin, timestamp_end)) {
            result += trade;
        }
        return result;
    }

    void cleanup() {
        size_t ignored_count = 0;
        size_t total_count = 0;
        for (const uint64_t deleted_order_id : deleted_orders_ids) {
            current_orders_t::accessor it;
            if (current_orders.find(it, deleted_order_id)) {
                std::vector<Order> orders;
                for (const Order& order : it->second) {
                    ++total_count;
                    if (order.timestamp > last_timestamp - _cleanup_threshold) {
                        ++ignored_count;
                        orders.push_back(order);
                    }
                }

                it->second = orders;
            }
            it.release();
        }
        std::cout << "CLEANUP: " << (total_count - ignored_count) << " / " << total_count << std::endl;
    }
    static void _cleanup(History* history) {
        while (history->_is_running) {
            std::this_thread::sleep_for(std::chrono::duration<double>(history->_cleanup_period));
            history->cleanup();
        }
    }

    inline void trade_insert(Trade trade) {
        if (trade.timestamp > last_timestamp) {
            last_timestamp = trade.timestamp;
        }
        trades.insert(trade);
        Order buy_order = order_get(trade.buy_order_id);
        if (buy_order.id) {
            orders.insert(buy_order);
        }
        Order sell_order = order_get(trade.sell_order_id);
        if (sell_order.id) {
            orders.insert(sell_order);
        }
    }

    inline void order_insert(const Order& order) {
        if (order.timestamp > last_timestamp) {
            last_timestamp = order.timestamp;
        }
        current_orders_t::accessor it;
        if (current_orders.find(it, order.id)) {
            it->second.push_back(order);
        } else {
            current_orders.insert({order.id, {order}});
        }
    }
    inline void order_update(const Order& order) {
        order_insert(order);
    }
    inline void order_delete(const Order& order) {
        current_orders_t::accessor it;
        if (current_orders.find(it, order.id)) {
            std::vector<Order> orders = it->second;
            bool is_found = false;
            for (const Order& o : orders) {
                if (o.amount == order.amount && o.type == order.type && o.price == order.price) {
                    is_found = true;
                    break;
                }
            }
            if (!is_found) {
                orders.push_back(order);
            }
        } else {
            current_orders.insert({order.id, {order}});
        }
        deleted_orders_ids.insert(order.id);
    }
    inline const Order order_get(const uint64_t& id, const double& timestamp) {
        Order result_order = {0};
        current_orders_t::accessor it;
        if (current_orders.find(it, id)) {
            for (const Order& order : it->second) {
                if (order.timestamp > result_order.timestamp && order.timestamp <= timestamp) {
                    result_order = order;
                }
            }
        }
        return result_order;
    }
    inline const Order order_get(const uint64_t& id) {
        return order_get(id, last_timestamp);
    }

    INDEXED_STORAGE_2(Trade, id, timestamp, RotatingLog, UpscaleBTree) trades;
    INDEXED_STORAGE(Order, id, RotatingLog, UpscaleBTree) orders;
    INDEXED_STORAGE(Decision, decision_timestamp, RotatingLog, UpscaleBTree) decisions;
    INDEXED_STORAGE(TradeAverage, timestamp_range, RotatingLog, UpscaleBTree) averages;
    typedef tbb::concurrent_hash_map<uint64_t, std::vector<Order>> current_orders_t;
    current_orders_t current_orders;
    std::set<uint64_t> deleted_orders_ids;
    double last_timestamp;

protected:
    const std::string _basepath;
    const std::string _currency_pair;
    bool _is_running;

private:
    double _cleanup_period;
    double _cleanup_threshold;
    std::thread _cleanup_thread;
};


#endif // CTRADING__HISTORY__HISTORY__HPP
