#include "bots/Bot.hpp"
#include "history/History.hpp"
#include "brokers/Broker.hpp"


int main(int argc, char const *argv[]) {
    History history("db/test-broker", "btceur");
    Bot bot;
    Broker broker(history, bot);
    broker.start(3);
    return 0;
}
