#ifndef CTRADING__BOTS__BOT__HPP
#define CTRADING__BOTS__BOT__HPP


#include "history/History.hpp"
#include "models/Advice.hpp"


class Bot {
public:

    inline Bot() {}
    inline ~Bot() {}

    virtual const Advice get_advice(const double& timestamp, History& history) {
        return Advice();
    }

};


#endif // CTRADING__BOTS__BOT__HPP
