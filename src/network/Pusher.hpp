#ifndef CTRADING__NETWORK__PUSHER__HPP
#define CTRADING__NETWORK__PUSHER__HPP


#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>

#include "json.hpp"
using JSON = nlohmann::json;

#include "./WebSocketClient.hpp"


typedef void (*pusher_callback_t) (const std::string&, const JSON&, void*);


struct PusherChannelSubscription {
    inline PusherChannelSubscription(const std::string& _channel_name, const pusher_callback_t _callback, void* _user_data) :
        channel_name(_channel_name),
        callback(_callback),
        user_data(_user_data) {}
    const std::string channel_name;
    const pusher_callback_t callback;
    void* user_data;
};


class PusherClient : public WebSocketClient {
public:

    inline PusherClient(const std::string& key, const std::string& cluster="mt1") :
        WebSocketClient("ws://ws-" + cluster + ".pusher.com/app/" + key + "?client=cpptrading&version=0.01&protocol=7"),
        _key(key),
        _cluster(cluster),
        _host("ws-" + _cluster + ".pusher.com"),
        _path("/app/" + _key + "?client=cpptrading&version=0.01&protocol=7")
    {}

    inline void send(const std::string& event, const JSON& data) {
        const std::string payload = JSON({
            {"event", event},
            {"data", data}
        }).dump();
        WebSocketClient::send(payload);
    }
    inline void subscribe(const std::string& channel_name, std::vector<std::string> events, pusher_callback_t callable, void* user_data=NULL) {
        // exit(0);
        for (const std::string& event : events) {
            _subscriptions.insert({event, {channel_name, callable, user_data}});
        }
    }
    inline void unsubscribe() {
        _subscriptions.clear();
    }

    inline void start() {
        // initialize websocket channels
        std::set<std::string> channel_names;
        for (const auto& it : _subscriptions) {
            channel_names.insert(it.second.channel_name);
        }
        WebSocketClient::start();
        for (const std::string& channel_name : channel_names) {
            send("pusher:subscribe", {{"channel", channel_name}});
        }
    }

    virtual void callback(const std::string& payload) {
        const auto message = JSON::parse(payload);
        try {
            const std::string event = message["event"];
            const auto data = JSON::parse(message["data"].get<std::string>());
            auto range = _subscriptions.equal_range(event);
            for (auto it=range.first; it!=range.second; ++it) {
                const PusherChannelSubscription& subsciption = it->second;
                subsciption.callback(event, data, subsciption.user_data);
            }
        } catch (const nlohmann::detail::type_error&) {
            std::cout << "UNEXPECTED PAYLOAD FORMAT: " << payload << std::endl;
        }
    }

private:
    const std::string _key;
    const std::string _cluster;
    const std::string _host;
    const std::string _path;
    std::multimap<std::string, PusherChannelSubscription> _subscriptions;
};


#endif // CTRADING__NETWORK__PUSHER__HPP
