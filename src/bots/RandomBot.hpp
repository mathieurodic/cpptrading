#ifndef CTRADING__BOTS__RANDOMBOT__HPP
#define CTRADING__BOTS__RANDOMBOT__HPP


#include "./Bot.hpp"

#include <stdlib.h>


class RandomBot : public Bot {
public:

    inline RandomBot(History& history, Broker& broker, const double& probabity=.1) :
        Bot(history, broker),
        _probabity(probabity),
        _last_price(NAN) {}

    virtual std::string get_name() const {
        return "RandomBot";
    }

    virtual Decision decide(const Timestamp& timestamp) {
        Decision decision;
        decision.price = _history.get_trade_summary(timestamp - 60., timestamp).average_price;
        if (std::isnan(decision.price)) {
            decision.price = _last_price;
        }
        _last_price = decision.price;
        if (rand() > _probabity * (double) RAND_MAX) {
            decision.type = WAIT;
        } else {
            decision.confidence = rand() / (double) RAND_MAX;
            decision.decision_timestamp = timestamp;
            decision.type = std::isnan(decision.price) ? WAIT : ((rand() % 2) ? BUY : SELL);
        }
        return decision;
    }

private:

    const double _probabity;
    double _last_price;

};


#endif // CTRADING__BOTS__RANDOMBOT__HPP
