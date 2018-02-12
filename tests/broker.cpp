#include <iostream>

#include "history/LogHistory.hpp"
#include "brokers/PretendBroker.hpp"


static const bool must_parse = false;


int main(int argc, char const *argv[]) {
    LogHistory log_history;
    PretendBroker broker(2000, 2.5e-3);
    broker.historize(log_history);

    std::cout << broker << '\n';
    {
        Decision decision;
        decision.type = BUY;
        decision.amount = .003;
        decision.price = 10000;
        broker.execute(decision);
        std::cout << broker << '\n';
    }
    {
        Decision decision;
        decision.type = BUY;
        decision.price = 10000;
        broker.execute(decision);
        std::cout << broker << '\n';
    }
    {
        Decision decision;
        decision.type = WAIT;
        broker.execute(decision);
        std::cout << broker << '\n';
    }
    {
        Decision decision;
        decision.type = SELL;
        decision.price = 12345;
        broker.execute(decision);
        std::cout << broker << '\n';
    }
    return 0;
}
