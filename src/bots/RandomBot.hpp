#ifndef CTRADING__BOTS__RANDOMBOT__HPP
#define CTRADING__BOTS__RANDOMBOT__HPP


#include "./Bot.hpp"

#include <stdlib.h>


class RandomBot : public Bot {
public:

    inline RandomBot(History& history, Broker& broker) :
        Bot(history, broker),
        _price(10000.) {}

    virtual Decision decide(const Timestamp& timestamp) {
        _price += (rand() / (double) RAND_MAX) - .5;
        Decision decision;
        decision.type = (ActionType) (rand() % 3 - 1);
        if (rand() % 2) {
            decision.type = WAIT;
        }
        decision.confidence = rand() / (double) RAND_MAX;
        decision.decision_timestamp = timestamp;
        decision.price = _price;
        return decision;
    }

private:

    double _price;

};


#endif // CTRADING__BOTS__RANDOMBOT__HPP
