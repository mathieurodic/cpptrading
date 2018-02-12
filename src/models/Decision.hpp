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
        status(PENDING),
        action_timestamp(NAN),
        execution_timestamp(NAN),
        confidence(NAN),
        order_id(0),
        amount(NAN),
        price(NAN),
        source{0} {}

    inline const bool operator==(const Decision& other) const { return memcmp(this, &other, sizeof(*this)) == 0; }
    inline const bool operator!=(const Decision& other) const { return memcmp(this, &other, sizeof(*this)) != 0; }

    inline operator bool () const {
        return type != WAIT;
    }

    inline const bool parse(const std::string& source) {
        return false;
    }

    ActionType type;
    int key;
    enum {
        CANCELLED = -1,
        PENDING = 0,
        PASSED = 1
    } status;
    Timestamp decision_timestamp;
    Timestamp action_timestamp;
    Timestamp execution_timestamp;
    double confidence;
    double amount;
    double price;
    uint64_t order_id;
    char source[32];
};

#pragma pack(pop)


#include <ostream>
#include <iomanip>
#include <string>

inline std::ostream& operator << (std::ostream& os, const Decision& decision) {
    std::string status;
    switch (decision.status) {
        case Decision::CANCELLED: status = "CANCELLED"; break;
        case Decision::PENDING:   status = "PENDING  "; break;
        case Decision::PASSED:    status = "PASSED   "; break;
    }
    return (os
        << "<Decision"
        << " type=" << decision.type
        << " status=" << decision.status
        << " decision_timestamp=" << Timestamp(decision.decision_timestamp)
        << " action_timestamp=" << Timestamp(decision.action_timestamp)
        << " execution_timestamp=" << Timestamp(decision.execution_timestamp)
        << " price=" << decision.price
        << " amount=" << decision.amount
        << " order_id=" << decision.order_id
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
