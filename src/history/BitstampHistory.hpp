#ifndef CTRADING__HISTORY__BITSTAMPHISTORY__HPP
#define CTRADING__HISTORY__BITSTAMPHISTORY__HPP


#include <set>

#include "./History.hpp"
#include "network/Pusher.hpp"
#include "network/RestClient.hpp"

#include "json.hpp"
using JSON = nlohmann::json;


class BitstampHistory : public History {
public:

    inline BitstampHistory(const std::string& basepath, const std::string& currency_pair) :
        History(basepath, currency_pair),
        _key("de504dc5763aeef9ff52"),
        _pusher_connection(_key),
        _rest_client("https://www.bitstamp.net/api/v2")
    {
        _pusher_connection.subscribe("live_trades_" + _currency_pair, {"trade"}, trades_callback, this);
        _pusher_connection.subscribe("live_orders_" + _currency_pair, {"order_created", "order_changed", "order_deleted"}, orders_callback, this);
        _pusher_connection.start();
    }

    static const Trade make_trade(const JSON& data) {
        return {
            .id = data["id"],
            .volume = data["amount"],
            .price = data["price"],
            .type = (data["type"] == 0) ? BUY : SELL,
            .timestamp = std::stod(data["timestamp"].get<std::string>()),
            .buy_order_id = data["buy_order_id"],
            .sell_order_id = data["sell_order_id"],
        };
    }
    static const Order make_order(const JSON& data) {
        return {
            .id = data["id"],
            .amount = data["amount"],
            .price = data["price"],
            .type = (data["order_type"] == 0) ? BUY : SELL,
            .timestamp = std::stod(data["datetime"].get<std::string>()),
        };
    }

    static void orders_callback(const std::string& event, const JSON& data, void* user_data) {
        Order order;
        try {
            order = make_order(data);
        } catch (const KeyException&) {
            std::cerr << "ERROR WHILE PARSING ORDER FROM BITSTAMP PUSHER: " << data << std::endl;
            return;
        } catch (const nlohmann::detail::type_error&) {
            std::cerr << "ERROR WHILE PARSING ORDER FROM BITSTAMP PUSHER: " << data << std::endl;
            return;
        }
        BitstampHistory& history = * (BitstampHistory*) user_data;

        // auto missing_order_iterator = history._missing_order_id.find(order.id);
        // if (missing_order_iterator != history._missing_order_id.end()) {
        //     std::cout << '\t' << order << std::endl;
        //     history._missing_order_id.erase(missing_order_iterator);
        // }

        std::cout << "[" << event << "] " << order << std::endl;
        if (event == "order_created") {
            history.order_insert(order);
        } else if (event == "order_changed") {
            history.order_update(order);
        } else if (event == "order_deleted") {
            history.order_delete(order);
        }
    }
    static void trades_callback(const std::string& event, const JSON& data, void* user_data) {
        Trade trade;
        try {
            trade = make_trade(data);
        } catch (const KeyException&) {
            std::cerr << "ERROR WHILE PARSING TRADE FROM BITSTAMP PUSHER: " << data << std::endl;
            return;
        } catch (const nlohmann::detail::type_error&) {
            std::cerr << "ERROR WHILE PARSING TRADE FROM BITSTAMP PUSHER: " << data << std::endl;
            return;
        }
        BitstampHistory& history = * (BitstampHistory*) user_data;
        //
        history.trade_insert(trade);
        Order buy_order = history.order_get(trade.buy_order_id);
        Order sell_order = history.order_get(trade.sell_order_id);
        std::cout << trade << std::endl;
        std::cout << '\t' << buy_order << std::endl;
        std::cout << '\t' << sell_order << std::endl;
    }

private:

    const std::string _key;
    PusherClient _pusher_connection;
    RestClient _rest_client;
};


#endif // CTRADING__HISTORY__BITSTAMPHISTORY__HPP
