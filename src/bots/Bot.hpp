#ifndef CTRADING__BOTS__BOT__HPP
#define CTRADING__BOTS__BOT__HPP


#include <thread>
#include <string>

#include "sources/Source.hpp"
#include "history/History.hpp"
#include "models/Decision.hpp"
#include "models/Timestamp.hpp"
#include "brokers/Broker.hpp"


class Bot {
public:

    inline Bot(History& history, Broker& broker) :
        _history(history),
        _broker(broker)
    {}
    inline ~Bot() {}

    virtual std::string get_name() const {
        return "BOT";
    }

    virtual Decision decide(const Timestamp& timestamp) = 0;

    void decide_and_execute(const Timestamp& timestamp) {
        Decision decision = decide(timestamp);
        _broker.execute(decision);
    }

protected:
    History& _history;

private:
    Broker& _broker;
    bool _is_running;
    std::thread _thread;
    double _interval;
};


#endif // CTRADING__BOTS__BOT__HPP
