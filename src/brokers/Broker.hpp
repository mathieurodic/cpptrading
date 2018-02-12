#ifndef CTRADING__BROKERS__BROKER__HPP
#define CTRADING__BROKERS__BROKER__HPP


#include <set>


class Broker {
public:

    inline Broker() :
        _commission(0.) {}

    virtual const std::string get_name() const {
        return "Broker";
    }

    inline void historize(History& history) {
        _histories.insert(&history);
    }

    virtual void send(Decision& decision) = 0;

    const bool validate(Decision& decision) {
        if (decision.type == WAIT) {
            return false;
        }
        if (std::isnan(decision.price)) {
            return false;
        }
        if (std::isnan(decision.amount)) {
            switch (decision.type) {
                case BUY:
                    decision.amount = _balance / decision.price;
                    break;
                case SELL:
                    decision.amount = _stock_balance;
                    break;
            }
        }
        if (decision.amount == 0.) {
            return false;
        }
        return true;
    }

    void feed(Decision& decision) {
        for (auto history : _histories) {
            history->feed(decision);
        }
    }

    void execute(Decision& decision) {
        Decision original_decision = decision;
        if (validate(decision)) {
            send(decision);
        } else {
            decision.status = Decision::CANCELLED;
        }
        if (decision.status == Decision::PASSED) {
            switch (decision.type) {
                case BUY:
                    this->_balance -= decision.amount * decision.price;
                    this->_stock_balance += decision.amount * (1. - _commission);
                    break;
                case SELL:
                    this->_balance += decision.amount * decision.price * (1. - _commission);
                    this->_stock_balance -= decision.amount;
                    break;
            }
        }
        if (original_decision != decision) {
            feed(original_decision);
        }
        feed(decision);
    }

    inline const double& get_balance() {
        return _balance;
    }
    inline const double& get_stock_balance() {
        return _stock_balance;
    }

protected:

    std::set<History*> _histories;
    double _balance;
    double _stock_balance;
    double _commission;

};


#include <ostream>

inline std::ostream& operator<<(std::ostream& os, Broker& broker) {
    return (os
        << "<" << broker.get_name()
        << " balance=" << broker.get_balance()
        << " stock_balance=" << broker.get_stock_balance()
        << ">"
    );
}


#endif // CTRADING__BROKERS__BROKER__HPP
