#ifndef CTRADING__MODELS__ORDER__HPP
#define CTRADING__MODELS__ORDER__HPP


#include <stdint.h>

#include "./ActionType.hpp"


#pragma pack(push, 1)

struct Order {
    inline const bool operator==(const Order& other) { return memcmp(this, &other, sizeof(*this)) == 0; }
    uint64_t id;
    double amount;
    double price;
    ActionType type;
    double timestamp;
};

#pragma pack(pop)


#endif // CTRADING__MODELS__ORDER__HPP
