#include <iostream>

#include "network/TCPServer.hpp"


class TCPSpecialHandler : public TCPHandler {
public:

    inline TCPSpecialHandler(size_t counter=0) : _counter(counter) {}

    virtual bool callback(TCPConnection& connection) {
        char data[4];
        std::cout << "READ" << '\n';
        connection.read(data, 4);
        std::cout << "WRITE" << '\n';
        connection.write("'");
        connection.write(data, 4);
        connection.write("'\n");
        connection.write("Request #" + std::to_string(++_counter) + "\n");
        return true;
    }

private:
    size_t _counter;
};


int main(int argc, char const *argv[]) {
    TCPSpecialHandler handler(7);
    TCPServer server(handler, 56789, 10);
    getchar();
    return 0;
}
