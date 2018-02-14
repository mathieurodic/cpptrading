#ifndef CTRADING__MODELS__TRADE__HPP
#define CTRADING__MODELS__TRADE__HPP


#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>

#include "./ActionType.hpp"
#include "./Timestamp.hpp"


#pragma pack(push, 1)

struct Trade {

    inline Trade() :
        id(0),
        volume(NAN),
        price(NAN),
        type(UNDEFINED),
        buy_order_id(0),
        sell_order_id(0) {}

    inline const bool operator==(const Trade& other) const {
        return memcmp(this, &other, sizeof(*this)) == 0;
    }
    inline const bool operator < (const Trade& other) const {
        return memcmp(this, &other, sizeof(*this)) < 0;
    }

    inline const bool parse(const std::string& source) {
        struct tm t;
        char tmp_type[8];
        int year;
        int result = sscanf(source.c_str(), "<Trade id=%" PRIu64 " buy_order_id=%" PRIu64 " sell_order_id=%" PRIu64 " timestamp=%d-%d-%dT%d:%d:%d type=%c%c%c%c price=%lf volume=%lf>",
            &id,
            &buy_order_id,
            &sell_order_id,
            &year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec,
            tmp_type, tmp_type+1, tmp_type+2, tmp_type+3,
            &price,
            &volume
        );
        if (result != 15) {
            return false;
        }
        t.tm_year = year - 1900;
        t.tm_mon -= 1;
        timestamp = t;
        switch (tmp_type[0]) {
            case 'W':
                type = WAIT;
                break;
            case 'S':
                type = SELL;
                break;
            case 'B':
                type = BUY;
                break;
            default:
                return false;
        }
        return true;
    }

    inline const Trade operator * (const double& factor) const {
        Trade trade = *this;
        trade.volume *= factor;
        return trade;
    }
    inline const Trade operator / (const double& factor) const {
        Trade trade = *this;
        trade.volume /= factor;
        return trade;
    }

    uint64_t id;
    double volume;
    double price;
    ActionType type;
    Timestamp timestamp;
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
        << " timestamp=" << Timestamp(trade.timestamp)
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
