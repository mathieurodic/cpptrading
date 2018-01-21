#ifndef CTRADING__MODELS__TRADE__HPP
#define CTRADING__MODELS__TRADE__HPP


#include <stdint.h>

#include "./ActionType.hpp"


#pragma pack(push, 1)

struct Trade {
    inline const bool operator==(const Trade& other) { return memcmp(this, &other, sizeof(*this)) == 0; }
    uint64_t id;
    double volume;
    double price;
    ActionType type;
    double timestamp;
    uint64_t buy_order_id;
    uint64_t sell_order_id;
};

#pragma pack(pop)


#endif // CTRADING__MODELS__TRADE__HPP
