#include "network/WebSocketClient.hpp"


class Echo : public WebSocketClient {
public:

    inline Echo() : WebSocketClient("ws://echo.websocket.org") {}

    virtual void callback(const std::string& payload) {
        if (payload == "azertyuiop") {
            send("qsdfghjklm");
        } else if (payload == "qsdfghjklm") {
            send("wxcvbn");
        }
    }

};


int main(int argc, char const *argv[]) {
    Echo ws;
    ws.set_debug(true);
    ws.start();
    ws.send("azertyuiop");
    getchar();
    return 0;
}
