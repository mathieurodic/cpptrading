#ifndef CTRADING__HISTORY__HISTORY__HPP
#define CTRADING__HISTORY__HISTORY__HPP


#include <models/Trade.hpp>
#include <models/Order.hpp>


class History {
public:

    virtual void record(Trade& trade) = 0;
    virtual void record(Order& order) = 0;

};


#endif // CTRADING__HISTORY__HISTORY__HPP
