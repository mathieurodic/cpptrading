#ifndef CTRADING__MODELS__ACTIONTYPE__HPP
#define CTRADING__MODELS__ACTIONTYPE__HPP


enum ActionType {
    BUY = +1,
    BID = +1,
    WAIT = 0,
    SELL = -1,
    ASK = -1,
};


#include <ostream>

inline std::ostream& operator << (std::ostream& os, const ActionType type) {
    switch (type) {
        case BUY: return (os << "BUY ");
        case WAIT: return (os << "WAIT");
        case SELL: return (os << "SELL");
        default: return (os << "????");
    }
}


#endif // CTRADING__MODELS__ACTIONTYPE__HPP
