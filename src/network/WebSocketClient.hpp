#ifndef CTRADING__NETWORK__WEBSOCKETCLIENT__HPP
#define CTRADING__NETWORK__WEBSOCKETCLIENT__HPP


#include <string>
#include <thread>

#include "./easywsclient.hpp"
#include "./easywsclient.cpp"
#include "exceptions/Exception.hpp"


class WebSocketClientException : public NetworkException {
public:
    using NetworkException::NetworkException;
};


class WebSocketClient {
public:

    inline WebSocketClient(const std::string& url, const std::string& origin="") :
        _debug(false),
        _url(url),
        _origin(origin),
        _ws(NULL),
        _thread(NULL),
        _is_running(false) {}
    inline ~WebSocketClient() {
        stop();
    }

    inline void set_debug(const bool debug) {
        _debug = debug;
    }

    static void _start(WebSocketClient* client) {
        while (client->_is_running) {
            client->_ws->poll(1);
            client->_ws->dispatch(client->_callback, client);
        }
    }
    inline void start() {
        stop();
        _is_running = true;
        // initialize websocket
        _ws = easywsclient::WebSocket::from_url(_url, _origin);
        if (_ws == NULL) {
            throw WebSocketClientException("Could not connect to websocket", _url);
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

    inline void send(const std::string& payload) {
        if (_debug) {
            std::cout << "WEBSOCKETCLIENT CLIENT SAYS: " << payload << '\n';
        }
        _ws->send(payload);
    }

    virtual void callback(const std::string& payload) = 0;
    static void _callback(const std::string& payload, void* _client) {
        WebSocketClient* client = (WebSocketClient*) _client;
        if (client->_debug) {
            std::cout << "WEBSOCKETCLIENT SERVER SAYS: " << payload << '\n';
        }
        client->callback(payload);
    }

private:

    bool _debug;
    const std::string _url;
    const std::string _origin;
    easywsclient::WebSocket::pointer _ws;
    std::thread* _thread;
    bool _is_running;
};


#endif // CTRADING__NETWORK__WEBSOCKETCLIENT__HPP
