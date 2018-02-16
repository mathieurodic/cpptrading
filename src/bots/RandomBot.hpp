#ifndef CTRADING__BOTS__RANDOMBOT__HPP
#define CTRADING__BOTS__RANDOMBOT__HPP


#include "./Bot.hpp"

#include <stdlib.h>


class RandomBot : public Bot {
public:

    inline RandomBot(History& history, Broker& broker, const double& probabity=.1) :
        Bot(history, broker),
        _probabity(probabity),
        _price(10000.) {}

    virtual std::string get_name() const {
        return "RandomBot";
    }

    virtual Decision decide(const Timestamp& timestamp) {
        _price += (rand() / (double) RAND_MAX) - .5;
        Decision decision;
        if (rand() > _probabity * (double) RAND_MAX) {
            decision.type = WAIT;
        } else {
            decision.confidence = rand() / (double) RAND_MAX;
            decision.decision_timestamp = timestamp;
            decision.price = _price;
            decision.type = (rand() % 2) ? BUY : SELL;
        }
        return decision;
    }

private:

    double _price;
    const double _probabity;

};


#endif // CTRADING__BOTS__RANDOMBOT__HPP
