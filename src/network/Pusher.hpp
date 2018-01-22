#ifndef CTRADING__NETWORK__PUSHER__HPP
#define CTRADING__NETWORK__PUSHER__HPP


#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>

#include "./easywsclient.hpp"
#include "./easywsclient.cpp"
#include "json.hpp"

using JSON = nlohmann::json;

#include "../exceptions/Exception.hpp"


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


class PusherConnection {
public:

    inline PusherConnection(const std::string& key, const std::string& cluster="mt1") :
        _key(key),
        _cluster(cluster),
        _host("ws-" + cluster + ".pusher.com"),
        _path("/app/" + key + "?client=cpptrading&version=0.01&protocol=7"),
        _url("ws://" + _host + _path),
        _ws(NULL),
        _thread(NULL),
        _is_running(false)
    {}

    inline ~PusherConnection() {
        stop();
    }

    inline void send(const std::string& event, const JSON& data) {
        const std::string payload = JSON({
            {"event", event},
            {"data", data}
        }).dump();
        _ws->send(payload);
    }
    inline void subscribe(const std::string& channel_name, std::vector<std::string> events, pusher_callback_t callable, void* user_data=NULL) {
        for (const std::string& event : events) {
            _subscriptions.insert({event, {channel_name, callable, user_data}});
        }
    }

    static void _start(PusherConnection* pusher_connection) {
        while (pusher_connection->_is_running) {
            pusher_connection->_ws->poll(1);
            pusher_connection->_ws->dispatch(pusher_connection->_callback, pusher_connection);
        }
    }
    inline void start() {
        stop();
        _is_running = true;
        // initialize websocket
        _ws = easywsclient::WebSocket::from_url(_url);
        if (_ws == NULL) {
            throw NetworkException("Could not connect to Pusher WebSocket", _key, _cluster);
        }
        // initialize websocket channels
        std::set<std::string> channel_names;
        for (const auto& it : _subscriptions) {
            channel_names.insert(it.second.channel_name);
        }
        for (const std::string& channel_name : channel_names) {
            send("pusher:subscribe", {{"channel", channel_name}});
        }
        // initialize thread
        _thread = new std::thread(_start, this);
    }
    inline void stop() {
        _is_running = false;
        // stop thread
        if (_thread) {
            _thread->join();
            delete _thread;
            _thread = NULL;
        }
        // stop websocket
        if (_ws) {
            _ws->close();
            delete _ws;
            _ws = NULL;
        }
    }

    void callback(const std::string& payload) {
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
    static void _callback(const std::string& payload, void* _pusher_connection) {
        PusherConnection& pusher_connection = * (PusherConnection*) _pusher_connection;
        pusher_connection.callback(payload);
    }

private:
    bool _is_running;
    const std::string _key;
    const std::string _cluster;
    const std::string _host;
    const std::string _path;
    const std::string _url;
    easywsclient::WebSocket::pointer _ws;
    std::thread* _thread;
    std::multimap<std::string, PusherChannelSubscription> _subscriptions;
};


#endif // CTRADING__NETWORK__PUSHER__HPP
