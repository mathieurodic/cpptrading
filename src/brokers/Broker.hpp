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
        if (std::isnan(decision.amount) && std::isnan(decision.stock_amount)) {
            switch (decision.type) {
                case BUY:
                    decision.amount = _balance;
                    break;
                case SELL:
                    decision.stock_amount = _stock_balance;
                    break;
            }
        }
        if (std::isnan(decision.amount)) {
            decision.amount = decision.stock_amount * decision.price;
        }
        if (std::isnan(decision.stock_amount)) {
            decision.stock_amount = decision.amount / decision.price;
        }
        if (decision.type == BUY && decision.amount > _balance) {
            return false;
        }
        if (decision.type == SELL && decision.stock_amount > _stock_balance) {
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
            decision.cancelled = true;
        }
        if (!decision.cancelled) {
            switch (decision.type) {
                case BUY:
                    this->_balance -= decision.amount;
                    this->_stock_balance += decision.stock_amount * (1. - _commission);
                    break;
                case SELL:
                    this->_balance += decision.amount * (1. - _commission);
                    this->_stock_balance -= decision.stock_amount;
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
