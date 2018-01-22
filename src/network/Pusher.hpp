#ifndef CTRADING__NETWORK__PUSHER__HPP
#define CTRADING__NETWORK__PUSHER__HPP


#include <string>
#include <vector>
#include <map>

#include "./easywsclient.hpp"
#include "./easywsclient.cpp"
#include "json.hpp"

using JSON = nlohmann::json;

#include "../exceptions/Exception.hpp"


typedef void (*pusher_callback_t) (const std::string&, const JSON&, void*);


class PusherConnection {
public:

    inline PusherConnection(const std::string& key, const std::string& cluster="mt1") :
        _key(key),
        _host("ws-" + cluster + ".pusher.com"),
        _path("/app/" + key + "?client=cpptrading&version=0.01&protocol=7"),
        _url("ws://" + _host + _path),
        _ws(easywsclient::WebSocket::from_url(_url))
    {
        if (_ws == NULL) {
            throw NetworkException("Could not connect to Pusher", _key, cluster);
        }
    }

    inline ~PusherConnection() {
        if (_ws) {
            _ws->close();
            delete _ws;
            _ws = NULL;
        }
    }

    inline void send(const std::string& event, const JSON& data) {
        const std::string payload = JSON({
            {"event", event},
            {"data", data}
        }).dump();
        _ws->send(payload);
    }
    inline void subscribe(const std::string& channel_name, std::vector<std::string> events, pusher_callback_t callable, void* user_data=NULL) {
        send("pusher:subscribe", {{"channel", channel_name}});
        for (const std::string& event : events) {
            _callbacks.insert({event, {callable, user_data}});
        }
    }

    inline void start() {
        while (_ws) {
            _ws->poll(1);
            _ws->dispatch(_callback, this);
        }
    }

    void callback(const std::string& payload) {
        const auto message = JSON::parse(payload);
        try {
            const std::string event = message["event"];
            const auto data = JSON::parse(message["data"].get<std::string>());
            auto range = _callbacks.equal_range(event);
            for (auto it=range.first; it!=range.second; ++it) {
                it->second.first(event, data, it->second.second);
            }
        } catch (const nlohmann::detail::type_error&) {
            std::cout << "UNEXPECTED PAYLOAD FORMAT" << std::endl;
        }
    }
    static void _callback(const std::string& payload, void* _pusher_connection) {
        PusherConnection& pusher_connection = * (PusherConnection*) _pusher_connection;
        pusher_connection.callback(payload);
    }

private:
    const std::string _key;
    const std::string _host;
    const std::string _path;
    const std::string _url;
    easywsclient::WebSocket::pointer _ws;
    std::multimap<std::string, std::pair<pusher_callback_t, void*>> _callbacks;
};


#endif // CTRADING__NETWORK__PUSHER__HPP
