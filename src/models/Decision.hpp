#ifndef CTRADING__MODELS__DECISION__HPP
#define CTRADING__MODELS__DECISION__HPP


#include <stdint.h>
#include <string.h>

#include "ActionType.hpp"


#pragma pack(push, 1)

struct Decision {
    inline const bool operator==(const Decision& other) { return memcmp(this, &other, sizeof(*this)) == 0; }
    uint64_t id;
    double amount;
    double price;
    ActionType type;
    double timestamp;
    uint64_t order_id;
    uint64_t trade_id;
};

#pragma pack(pop)


#endif // CTRADING__MODELS__DECISION__HPP
