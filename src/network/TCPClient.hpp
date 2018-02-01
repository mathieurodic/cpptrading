#ifndef CPPTRAING__NETWORK___TCPCLIENT_HPP
#define CPPTRAING__NETWORK___TCPCLIENT_HPP


#include <string>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "exceptions/Exception.hpp"


class TCPClientError : public NetworkException {
public:
    using NetworkException::NetworkException;
};


class TCPClient {
public:
    inline TCPClient(const std::string& host, const int port, const double timeout=3.) :
        _host(host),
        _port(port),
        _timeout(timeout),
        _host_entry(gethostbyname(_host.c_str())),
        _sock(socket(AF_INET, SOCK_STREAM, 0))
    {
        if (_host_entry == NULL) {
            throw TCPClientError("TCPClient could not resolve host", hstrerror(h_errno), _host, _port);
        }
        if (_sock == -1) {
            throw TCPClientError("TCPClient could not initialize socket", strerror(errno), _host, _port);
        }
        _address.sin_family = AF_INET;      /* host byte order */
        _address.sin_port = htons(_port);    /* short, network byte order */
        _address.sin_addr = *((struct in_addr *)_host_entry->h_addr);
        memset(&(_address.sin_zero), 0, 8);
        //
        fcntl(_sock, F_SETFL, O_NONBLOCK);
        connect(_sock, (struct sockaddr *)&_address, sizeof(_address));
        //
        struct timeval tv;
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(_sock, &fdset);
        tv.tv_sec = (long) timeout;
        tv.tv_usec = ((long) (timeout * 1e6)) % 1000000;
        //
        // setsockopt(_sock, IPPROTO_TCP, TCP_SYNCNT, &synRetries, sizeof(synRetries));
        int result;
        result = select(_sock + 1, NULL, &fdset, NULL, &tv);
        if (result < 0) {
            throw TCPClientError("TCPClient could not select socket", strerror(errno), _host, _port);
        } else if (result == 0) {
            throw TCPClientError("TCPClient could not select socket", "timeout", _timeout, _host, _port);
        }
        //
        int so_error;
        socklen_t len = sizeof so_error;
        getsockopt(_sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error != 0) {
            throw TCPClientError("TCPClient could not open socket", strerror(errno), _host, _port);
        }
        //
        set_blocking(true);
    }

    inline ~TCPClient() {
        close(_sock);
    }

    inline void set_blocking(const bool is_blocking) {
        int oldfl;
        oldfl = fcntl(_sock, F_GETFL);
        fcntl(_sock, F_SETFL, oldfl & (is_blocking ? (~O_NONBLOCK) : O_NONBLOCK));
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
        if (is_closed()) {
            throw TCPClientError("TCPConnection got closed by client", _sock);
        }
    }

    inline int read(void* data, const size_t size) {
        int offset = 0;
        while (offset < size) {
            const int missing = size - offset;
            const int result = recv(_sock, (char*)data + offset, size, 0);
            if (result == -1) {
                throw TCPClientError("TCPConnection could not read", _sock, strerror(errno));
            }
            offset += result;
            ensure_is_not_closed();
        }
        return offset;
    }
    inline void read(std::string& str) {
        str.clear();
        int offset;
        do {
            offset = recv(_sock, _buffer, sizeof(_buffer), 0);
            if (offset == -1) {
                throw TCPClientError("TCPConnection could not read", _sock, strerror(errno));
            }
            str.append(_buffer, offset);
        } while (offset == sizeof(_buffer));
    }
    template <typename T>
    inline int read(T& object) {
        int result = read(&object, sizeof(object));
        return result;
    }

    inline void write(const std::string& str) {
        write(str.data(), str.size());
    }
    inline void write(const void* data, const size_t size) {
        int result = send(_sock, data, size, 0);
        if (result == -1) {
            throw TCPClientError("TCPClient could not write data", strerror(errno), size, _host, _port);
		}
    }
    template <typename T>
    inline void write(T& object) {
        return write(&object, sizeof(object));
    }

private:
    const std::string _host;
    const int _port;
    const double _timeout;
    struct hostent* _host_entry;
    int _sock;
    struct sockaddr_in _address;
    char _buffer[4096];
    std::string _read_buffer;
};


#endif // CPPTRAING__NETWORK___TCPCLIENT_HPP
