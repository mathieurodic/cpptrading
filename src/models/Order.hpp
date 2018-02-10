#ifndef CTRADING__MODELS__ORDER__HPP
#define CTRADING__MODELS__ORDER__HPP


#include <stdint.h>

#include "./ActionType.hpp"
#include "./Timestamp.hpp"


#pragma pack(push, 1)

struct Order {

    inline const bool operator==(const Order& other) const { return memcmp(this, &other, sizeof(*this)) == 0; }
    inline const bool parse(const std::string& source) {
        return false;
    }

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

namespace std {
    template<>
    struct hash<Order> {
        size_t operator()(const Order& order) const
        {
            static const size_t n = sizeof(Order) / sizeof(size_t);
            size_t result;
            for (size_t i=0; i<n; ++i) {
                result ^= * (((size_t*) &order) + i);
            }
            if (sizeof(Order) % sizeof(size_t)) {
                result ^= * (size_t*) (((char*) &order) + sizeof(Order) - sizeof(size_t));
            }
            return result;
        }
    };
}


#endif // CTRADING__MODELS__ORDER__HPP
