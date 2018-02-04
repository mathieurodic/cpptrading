#ifndef CPPTRAING__SOURCES__SOURCE_HPP
#define CPPTRAING__SOURCES__SOURCE_HPP


#include <stdint.h>

#include <set>
#include <thread>
#include <tbb/concurrent_hash_map.h>

#include "history/History.hpp"


class Source {
public:

    inline Source(const std::string& currency_pair) :
        _is_running(true),
        _last_timestamp(0),
        _currency_pair(currency_pair),
        _cleanup_period(10.0),
        _cleanup_threshold(60.0),
        _cleanup_thread(std::thread(_cleanup, this))
    {}

    inline ~Source() {
        _is_running = false;
        _cleanup_thread.join();
    }

    inline void historize(History& history) {
        _histories.insert(&history);
    }

    inline Order retrieve_order(const uint64_t& id, const double& timestamp) {
        Order result_order = {0};
        tbb::concurrent_hash_map<uint64_t, std::vector<Order>>::accessor it;
        if (_current_orders.find(it, id)) {
            for (const Order& order : it->second) {
                if (order.timestamp > result_order.timestamp && order.timestamp <= timestamp) {
                    result_order = order;
                }
            }
        }
        return result_order;
    }

    inline void feed(Trade& trade) {
        if (trade.timestamp > _last_timestamp) {
            _last_timestamp = trade.timestamp;
        }
        Order buy_order = retrieve_order(trade.buy_order_id, trade.timestamp);
        Order sell_order = retrieve_order(trade.sell_order_id, trade.timestamp);
        for (auto history : _histories) {
            history->feed(trade);
            if (sell_order.type != WAIT) {
                history->feed(buy_order);
            }
            if (sell_order.type != WAIT) {
                history->feed(sell_order);
            }
        }
    }
    inline void feed(Order& order) {
        if (order.timestamp > _last_timestamp) {
            _last_timestamp = order.timestamp;
        }
        tbb::concurrent_hash_map<uint64_t, std::vector<Order>>::accessor it;
        if (_current_orders.find(it, order.id)) {
            it->second.push_back(order);
        } else {
            _current_orders.insert({order.id, {order}});
        }
    }
    inline void puke(Order& order) {
        tbb::concurrent_hash_map<uint64_t, std::vector<Order>>::accessor it;
        if (!_current_orders.find(it, order.id)) {
            feed(order);
        }
        _deleted_orders_ids.insert(order.id);
    }

protected:

    const std::string _currency_pair;
    double _last_timestamp;

private:

    std::set<History*> _histories;
    tbb::concurrent_hash_map<uint64_t, std::vector<Order>> _current_orders;
    std::set<uint64_t> _deleted_orders_ids;

    void cleanup() {
        size_t ignored_count = 0;
        size_t total_count = 0;
        const size_t old_current_orders_size = _current_orders.size();
        for (const uint64_t deleted_order_id : _deleted_orders_ids) {
            tbb::concurrent_hash_map<uint64_t, std::vector<Order>>::accessor it;
            if (_current_orders.find(it, deleted_order_id)) {
                std::vector<Order> orders;
                for (const Order& order : it->second) {
                    ++total_count;
                    if (order.timestamp > _last_timestamp - _cleanup_threshold) {
                        ++ignored_count;
                        orders.push_back(order);
                    }
                }
                if (orders.size() == 0) {
                    _current_orders.erase(it);
                } else {
                    it->second = orders;
                }
            }
        }
        std::cout << "CLEANUP: " << (total_count - ignored_count) << " / " << total_count << "   [" << old_current_orders_size << "->" << _current_orders.size() <<"]" << std::endl;
    }
    static void _cleanup(Source* source) {
        while (source->_is_running) {
            std::this_thread::sleep_for(std::chrono::duration<double>(source->_cleanup_period));
            source->cleanup();
        }
    }

    bool _is_running;
    double _cleanup_period;
    double _cleanup_threshold;
    std::thread _cleanup_thread;
};


#endif // CPPTRAING__SOURCES__SOURCE_HPP
