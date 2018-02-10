#ifndef CTRADING__MODELS__TRADE__HPP
#define CTRADING__MODELS__TRADE__HPP


#include <stdint.h>

#include "./ActionType.hpp"
#include "./Timestamp.hpp"


#pragma pack(push, 1)

struct Trade {
    inline const bool operator==(const Trade& other) const { return memcmp(this, &other, sizeof(*this)) == 0; }
    uint64_t id;
    double volume;
    double price;
    ActionType type;
    double timestamp;
    uint64_t buy_order_id;
    uint64_t sell_order_id;
};

#pragma pack(pop)


#include <ostream>
#include <iomanip>
#include <string>

inline std::ostream& operator << (std::ostream& os, const Trade& trade) {
    std::string decimals = std::to_string((int)(100.0 * trade.price) % 100);
    if (decimals.size() < 2) {
        decimals += '0';
    }
    return (os
        << "<Trade"
        << " id=" << trade.id
        << " buy_order_id=" << trade.buy_order_id
        << " sell_order_id=" << trade.sell_order_id
        << " timestamp=" << Timestamp<double>(trade.timestamp)
        << " type=" << trade.type
        << " price=" << (int)(trade.price) << '.' << decimals
        << " volume=" << trade.volume
        << ">"
    );
}


namespace std {
    template<>
    struct hash<Trade> {
        size_t operator()(const Trade& trade) const
        {
            static const size_t n = sizeof(Trade) / sizeof(size_t);
            size_t result;
            for (size_t i=0; i<n; ++i) {
                result ^= * (((size_t*) &trade) + i);
            }
            if (sizeof(Trade) % sizeof(size_t)) {
                result ^= * (size_t*) (((char*) &trade) + sizeof(Trade) - sizeof(size_t));
            }
            return result;
        }
    };
}


#endif // CTRADING__MODELS__TRADE__HPP
