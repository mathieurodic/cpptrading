#include <iostream>

#include "network/TCPServer.hpp"
#include "network/TCPClient.hpp"


class TCPSpecialHandler : public TCPHandler {
public:

    inline TCPSpecialHandler(size_t counter=0) : _counter(counter) {}

    virtual bool callback(TCPConnection& connection) {
        size_t nonce;
        connection.read(nonce);
        connection.write(nonce);
        for (int i=0, n=rand()%16; i<n; ++i) {
            connection.write('\1');
            connection.write(_counter++);
        }
        connection.write('\0');
        return true;
    }

private:
    size_t _counter;
};


int main(int argc, char const *argv[]) {

    TCPSpecialHandler handler(42);
    TCPServer server(handler, "0.0.0.0", 7890, 1);

    for (int i=0; i<1; ++i) {
        usleep(100000);
        TCPClient client("127.0.0.1", 7890);
        size_t n = rand();
        char continuation;
        //
        client.write(n);
        std::cout << "sent nonce = " << n << '\n';
        n = 0;
        client.read(n);
        std::cout << "recieved nonce = " << n << '\n';
        while (true) {
            client.read(continuation);
            std::cout << "recieved continuation = " << (int)continuation << '\n';
            if (continuation) {
                client.read(n);
                std::cout << "recieved counter = " << n << '\n';
            } else {
                break;
            }
        }
        // std::cout << '\n';
        usleep(100000);
    }

    // getchar();
    return 0;
}
