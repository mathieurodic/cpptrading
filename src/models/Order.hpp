#ifndef CTRADING__MODELS__ORDER__HPP
#define CTRADING__MODELS__ORDER__HPP


#include <stdint.h>

#include "./ActionType.hpp"
#include "./Timestamp.hpp"


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


#include <ostream>
#include <iomanip>
#include <string>

inline std::ostream& operator << (std::ostream& os, const Order& order) {
    std::string decimals = std::to_string((int)(100.0 * order.price) % 100);
    if (decimals.size() < 2) {
        decimals += '0';
    }
    return (os
        << "<Order"
        << " id=" << order.id
        << " timestamp=" << Timestamp<double>(order.timestamp)
        << " type=" << order.type
        << " price=" << (int)(order.price) << '.' << decimals
        << " amount=" << order.amount
        << ">"
    );
}


#endif // CTRADING__MODELS__ORDER__HPP
