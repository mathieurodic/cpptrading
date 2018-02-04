#include <iostream>

#include "network/Pusher.hpp"


void trades_callback(const std::string& event, const JSON& data, void* user_data) {
    std::cout << "THIS IS A TRADE: " << event << ": " << data << std::endl;
}
void trades_callback_2(const std::string& event, const JSON& data, void* user_data) {
    std::cout << "THIS IS THE SAME TRADE" << std::endl;
}
void orders_callback(const std::string& event, const JSON& data, void* user_data) {
    std::cout << "THIS IS AN ORDER: " << event << ": " << data << std::endl;
}


#include <thread>
#include <chrono>

int main(int argc, char const *argv[]) {
    PusherClient client("de504dc5763aeef9ff52");
    client.set_debug(true);
    client.subscribe("live_trades_btceur", {"trade"}, trades_callback);
    client.subscribe("live_trades_btceur", {"trade"}, trades_callback_2);
    client.subscribe("live_orders_btceur", {"order_created", "order_changed", "order_deleted"}, orders_callback);
    client.start();
    getchar();
    return 0;
}
