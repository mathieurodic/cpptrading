#ifndef CTRADING__MODELS__DECISION__HPP
#define CTRADING__MODELS__DECISION__HPP


#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ActionType.hpp"


#pragma pack(push, 1)

struct Decision {
    inline Decision() :
        type(WAIT),
        key(rand()),
        decision_timestamp(time(NULL)),
        action_timestamp(NAN),
        execution_timestamp(NAN),
        confidence(NAN),
        order_id(0),
        amount(0.0),
        minimum_price(NAN),
        maximum_price(NAN),
        source{0} {}
    inline const bool operator==(const Decision& other) const { return memcmp(this, &other, sizeof(*this)) == 0; }

    ActionType type;
    int key;
    double decision_timestamp;
    double action_timestamp;
    double execution_timestamp;
    double confidence;
    double amount;
    double minimum_price;
    double maximum_price;
    uint64_t order_id;
    char source[32];
};

#pragma pack(pop)


#include <ostream>
#include <iomanip>
#include <string>

inline std::ostream& operator << (std::ostream& os, const Decision& decision) {

    std::string max_decimals = std::to_string((int)(100.0 * decision.maximum_price) % 100);
    if (max_decimals.size() < 2) {
        max_decimals += '0';
    }
    (os
        << "<Decision"
        << " type=" << decision.type
        << " decision_timestamp=" << Timestamp<double>(decision.decision_timestamp)
        << " action_timestamp=" << Timestamp<double>(decision.action_timestamp)
        << " execution_timestamp=" << Timestamp<double>(decision.execution_timestamp)
    );
    os << " minimum_price=";
    if (std::isnan(decision.minimum_price)) {
        os << "????.??";
    } else {
        std::string decimals = std::to_string((int)(100.0 * decision.minimum_price) % 100);
        if (decimals.size() < 2) {
            decimals += '0';
        }
        os << (int)(decision.minimum_price) << '.' << decimals;
    }
    os << " maximum_price=";
    if (std::isnan(decision.maximum_price)) {
        os << "????.??";
    } else {
        std::string decimals = std::to_string((int)(100.0 * decision.maximum_price) % 100);
        if (decimals.size() < 2) {
            decimals += '0';
        }
        os << (int)(decision.maximum_price) << '.' << decimals;
    }
    os << " amount=";
    if (std::isnan(decision.amount)) {
        os << "?.????";
    } else {
        std::string decimals = std::to_string((int)(100.0 * decision.amount) % 100);
        if (decimals.size() < 2) {
            decimals += '0';
        }
        os << (int)(decision.amount) << '.' << decimals;
    }
    return (os
        << " order_id=" << decision.order_id
        << ">"
    );
}


namespace std {
    template<>
    struct hash<Decision> {
        size_t operator()(const Decision& decision) const
        {
            static const size_t n = sizeof(Decision) / sizeof(size_t);
            size_t result;
            for (size_t i=0; i<n; ++i) {
                result ^= * (((size_t*) &decision) + i);
            }
            if (sizeof(Decision) % sizeof(size_t)) {
                result ^= * (size_t*) (((char*) &decision) + sizeof(Decision) - sizeof(size_t));
            }
            return result;
        }
    };
}


#endif // CTRADING__MODELS__DECISION__HPP
