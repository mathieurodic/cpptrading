#ifndef CPPTRAING__SOURCES__BITSTAMPSOURCE_HPP
#define CPPTRAING__SOURCES__BITSTAMPSOURCE_HPP


#include "json.hpp"
using JSON = nlohmann::json;

#include "./Source.hpp"
#include "models/Trade.hpp"
#include "models/Order.hpp"
#include "network/Pusher.hpp"
#include "network/RestClient.hpp"

#include <string>


class BitstampSource : public Source {
public:

    inline BitstampSource(const std::string& currency_pair) :
        Source(currency_pair),
        _pusher_client("de504dc5763aeef9ff52"),
        _rest_client("https://www.bitstamp.net/api/v2")
    {
        _pusher_client.subscribe("live_trades_" + _currency_pair, {"trade"}, trades_callback, this);
        _pusher_client.subscribe("live_orders_" + _currency_pair, {"order_created", "order_changed", "order_deleted"}, orders_callback, this);
        _pusher_client.start();
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
        BitstampSource& source = * (BitstampSource*) user_data;
        // std::cout << "[" << event << "] " << order << std::endl;
        if (event == "order_created") {
            source.feed(order);
        } else if (event == "order_changed") {
            source.feed(order);
        } else if (event == "order_deleted") {
            source.puke(order);
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
        BitstampSource& source = * (BitstampSource*) user_data;
        //
        source.feed(trade);
    }

private:

    History* _history;
    PusherClient _pusher_client;
    RestClient _rest_client;

};


#endif // CPPTRAING__SOURCES__BITSTAMPSOURCE_HPP
