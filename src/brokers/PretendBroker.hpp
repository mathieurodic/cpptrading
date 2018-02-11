#ifndef CTRADING__BROKERS__PRETENDBROKER__HPP
#define CTRADING__BROKERS__PRETENDBROKER__HPP


#include "./Broker.hpp"


class PretendBroker : public Broker {
public:

    using Broker::_balance;
    using Broker::_commission;

    inline PretendBroker(const double balance, const double commission) {
        this->_balance = balance;
        this->_stock_balance = 0.;
        this->_commission = commission;
    }

    virtual const std::string get_name() const {
        return "PretendBroker";
    }

    virtual void send(Decision& decision) {
        decision.passed = true;
        decision.action_timestamp = decision.decision_timestamp;
        decision.execution_timestamp = decision.decision_timestamp;
    }

private:

};


#endif // CTRADING__BROKERS__PRETENDBROKER__HPP
