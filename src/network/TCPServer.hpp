#ifndef CPPTRAING__NETWORK___TCPSERVER_HPP
#define CPPTRAING__NETWORK___TCPSERVER_HPP


#include <netdb.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <thread>
#include <vector>
#include <list>

#include "exceptions/Exception.hpp"


class TCPConnectionClosedException : public NetworkException {
public:
    using NetworkException::NetworkException;
};


class TCPConnection;

class TCPHandler {
public:
    virtual bool callback(TCPConnection& connection) = 0;
};


class TCPConnection {
public:

    inline TCPConnection(TCPHandler& handler, const int sock, const time_t timeout=3) :
        _handler(handler),
        _sock(sock),
        _is_running(true),
        _thread(loop, this),
        _timeout(timeout),
        _endtime(timeout + time(NULL))
    {
        std::cout << "START CONNECTION " << _sock << '\n';
    }

    inline ~TCPConnection() {
        _is_running = false;
        _thread.join();
    }

    const bool is_closed() {
        fd_set rfd;
        FD_ZERO(&rfd);
        FD_SET(_sock, &rfd);
        timeval tv = { 0 };
        select(_sock+1, &rfd, 0, 0, &tv);
        if (!FD_ISSET(_sock, &rfd)) {
            return false;
        }
        int n = 0;
        ioctl(_sock, FIONREAD, &n);
        return n == 0;
    }
    void ensure_is_not_closed() {
        if (time(NULL) > _endtime) {
            _is_running = false;
            throw TCPConnectionClosedException("TCPConnection timed out", _sock);
        }
        if (is_closed()) {
            _is_running = false;
            throw TCPConnectionClosedException("TCPConnection got closed by client", _sock);
        }
    }

    inline void start_timeout() {
        _endtime = _timeout + time(NULL);
    }

    inline void write(const std::string& str) {
        write(str.data(), str.size());
    }
    inline void write(const void* data, const int size) {
        int offset = 0;
        start_timeout();
        while (offset < size) {
            const int result = send(_sock, (const char*)data + offset, size, 0);
            if (result == -1) {
                _is_running = false;
                throw TCPConnectionClosedException("TCPConnection could not write", _sock, strerror(errno));
            }
            offset += result;
            ensure_is_not_closed();
        }
    }
    template <typename T>
    inline void write(const T& object) {
        return write((void*) &object, sizeof(object));
    }

    inline void read(void* data, const int size) {
        int offset = 0;
        start_timeout();
        while (offset < size) {
            const int result = recv(_sock, (char*)data + offset, size, 0);
            if (result == -1) {
                _is_running = false;
                throw TCPConnectionClosedException("TCPConnection could not read", _sock, strerror(errno));
            }
            offset += result;
            ensure_is_not_closed();
        }
    }
    template <typename T>
    inline void read(const T& object) {
        return read((void*) &object, sizeof(object));
    }

    static void loop(TCPConnection* replier) {
        ssize_t r;
        std::string recieved_data;
        try {
            while (replier->_is_running && replier->_handler.callback(*replier)) {
                replier->ensure_is_not_closed();
                usleep(1);
            }
        } catch (TCPConnectionClosedException) {
            replier->_is_running = false;
        }
        close(replier->_sock);
        std::cout << "END CONNECTION " << replier->_sock << '\n';
    }

private:
    TCPHandler& _handler;
    const int _sock;
    bool _is_running;
    int _timeout;
    time_t _endtime;
    std::thread _thread;
};


class TCPServer {
public:

    inline TCPServer(TCPHandler& handler, const std::string& host, const int port, const int timeout=10) :
        _handler(handler),
        _is_running(true),
        _host(host),
        _port(port),
        _timeout(timeout),
        _thread(_start, this)
    {}

    inline ~TCPServer() {
        std::cout << "CLOSE SERVER #1" << '\n';
        _is_running = false;
        std::cout << "CLOSE SERVER #2" << '\n';
        _thread.join();
        std::cout << "CLOSE SERVER #3" << '\n';
        for (TCPConnection* connection : _connections) {
            delete connection;
        }
        std::cout << "CLOSE SERVER #4" << '\n';
    }

    static void _start(TCPServer* server) {
        server->start();
    }
    void start() {
        signal(SIGPIPE, SIG_IGN);
        int sock = make_accept_sock();
        while (_is_running) {
            struct timeval tv;
            tv.tv_sec = (long)_timeout;
            tv.tv_usec = 0;
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(sock, &rfds);
            if (select(sock + 1, &rfds, 0, 0, &tv)) {
                int new_sock = accept(sock, 0, 0);
                _connections.push_back(new TCPConnection(_handler, new_sock, _timeout));
            }
            usleep(1);
        }
    }

    int make_accept_sock() {
        const int one = 1;
        struct addrinfo hints = {};
        struct addrinfo *res = 0, *ai = 0, *ai4 = 0;
        int sock;

        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        getaddrinfo(_host.c_str(), std::to_string(_port).c_str(), &hints, &res);

        for (ai = res; ai; ai = ai->ai_next) {
            if (ai->ai_family == PF_INET6) break;
            else if (ai->ai_family == PF_INET) ai4 = ai;
        }
        ai = ai ? ai : ai4;

        sock = socket(ai->ai_family, SOCK_STREAM, 0);
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        bind(sock, ai->ai_addr, ai->ai_addrlen);
        listen(sock, 256);
        freeaddrinfo(res);
        return sock;
    }

private:
    TCPHandler& _handler;
    const std::string _host;
    const int _port;
    std::thread _thread;
    bool _is_running;
    int _timeout;
    std::list<TCPConnection*> _connections;
};


#endif // CPPTRAING__NETWORK___TCPSERVER_HPP
