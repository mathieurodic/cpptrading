#ifndef CTRADING__MODELS__BALANCE__HPP
#define CTRADING__MODELS__BALANCE__HPP


#include "./Timestamp.hpp"


#pragma pack(push, 1)


struct Balance {

    inline Balance(const double& _liquidity=NAN, const double& _stock=NAN, const double& _commission=NAN) :
        liquidity(_liquidity),
        stock(_stock),
        commission(_commission) {}

    inline Balance operator + (const Balance& other) const {
        return Balance(
            liquidity + other.liquidity,
            stock + other.stock,
            commission + other.commission
        );
    }

    double liquidity;
    double stock;
    double commission;

};


struct BalanceChangeOrigin {
    enum Type {
        UNKNOWN = 0,
        INIT = 1,
        UPDATE = 2,
        TRADE = 3,
    };

    inline BalanceChangeOrigin(Type _type=UNKNOWN, uint64_t _id=0) : type(_type), id(_id) {}

    Type type;
    uint64_t id;
};


struct BalanceChange {

    inline BalanceChange() :
        timestamp(NAN) {}

    BalanceChangeOrigin origin;
    Balance delta;
    Balance consolidated;
    Timestamp timestamp;

    inline const bool parse(const std::string& source) {
        return false;
    }

};


#pragma pack(pop)


#include <ostream>

std::ostream& operator << (std::ostream& os, const Balance& balance) {
    return (os
        << "("
        << "liquidity=" << balance.liquidity
        << " stock=" << balance.stock
        << " commission=" << balance.commission
        << ")"
    );
}

std::ostream& operator << (std::ostream& os, const BalanceChangeOrigin& balance_change_origin) {
    static const std::string type_labels[] = {"UNKNOWN", "INIT", "UPDATE", "TRADE"};
    os << type_labels[(int) balance_change_origin.type];
    if (balance_change_origin.id) {
        os << '-' << balance_change_origin.id;
    }
    return os;
}

std::ostream& operator << (std::ostream& os, const BalanceChange& balance_change) {
    return (os
        << "<BalanceChange"
        << " origin=" << balance_change.origin
        << " delta=" << balance_change.delta
        << " consolidated=" << balance_change.consolidated
        << " timestamp=" << balance_change.timestamp
        << ")"
    );
}


#endif // CTRADING__MODELS__BALANCE__HPP
