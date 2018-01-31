#include "network/TCPClient.hpp"


int main(int argc, char const *argv[]) {
    {
        // TCPClient client("www.example.org", 80);
        TCPClient client("localhost", 80);

        std::string request = "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n";
        client.write(request);

        std::string response;
        client.read(response);
        std::cout << response << std::endl;
    }
    // {
    //     TCPClient client("www.example.org", 18954);
    // }
    // {
    //     TCPClient client("this.domain.does.not.exist", 80);
    // }
    return 0;
}
