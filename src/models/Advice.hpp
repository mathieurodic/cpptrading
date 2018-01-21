#ifndef CTRADING__MODELS__ADVICE__HPP
#define CTRADING__MODELS__ADVICE__HPP


#include <math.h>
#include <stdint.h>
#include <string.h>

#include "./ActionType.hpp"


#pragma pack(push, 1)

struct Advice {
    inline Advice(const double& _timestamp_advice=NAN) :
        trade_id(0),
        order_id(0),
        volume(0.0),
        minimum_price(NAN),
        maximum_price(NAN),
        timestamp_advice(_timestamp_advice),
        timestamp_execution(NAN),
        confidence(0.0)
        {}
    inline const bool operator==(const Decision& other) { return memcmp(this, &other, sizeof(*this)) == 0; }
    uint64_t trade_id;
    uint64_t order_id;
    ActionType type;
    double volume;
    double minimum_price;
    double maximum_price;
    double timestamp_advice;
    double timestamp_execution;
    double confidence;
};

#pragma pack(pop)


#endif // CTRADING__MODELS__ADVICE__HPP
