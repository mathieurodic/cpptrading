#ifndef CTRADING__BOTS__BOT__HPP
#define CTRADING__BOTS__BOT__HPP


#include "history/History.hpp"
#include "models/Decision.hpp"


class Bot {
public:

    inline Bot() {}
    inline ~Bot() {}

    virtual Decision decide(History& history, const double& timestamp) {
        return Decision();
    }

};


#endif // CTRADING__BOTS__BOT__HPP
