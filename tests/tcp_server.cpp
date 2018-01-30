#include <iostream>

#include "network/TCPServer.hpp"


int main(int argc, char const *argv[]) {
    TCPServer server(56789, 3);
    getchar();
    return 0;
}
