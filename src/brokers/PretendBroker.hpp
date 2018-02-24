#ifndef CTRADING__BROKERS__PRETENDBROKER__HPP
#define CTRADING__BROKERS__PRETENDBROKER__HPP


#include "./Broker.hpp"
#include "models/Balance.hpp"


class PretendBroker : public Broker {
public:

    inline void init(const Timestamp& timestamp, const double& liquidity, const double& commission) {
        _commission = commission;
        BalanceChange balance_change;
        balance_change.timestamp = timestamp;
        balance_change.origin.type = BalanceChangeOrigin::INIT;
        balance_change.origin.id = 0;
        balance_change.consolidated.liquidity = liquidity;
        balance_change.consolidated.stock = 0.;
        balance_change.consolidated.commission = 0.;
        feed(balance_change);
    }

    virtual const double compute_commission(const Trade& trade) {
        return trade.price * trade.volume * _commission;
    }

    virtual const std::string get_name() const {
        return "PretendBroker";
    }

    virtual std::vector<Trade> send(Decision& decision) {
        decision.status = Decision::PASSED;
        Trade trade;
        trade.timestamp = decision.timestamp;
        trade.decision_id = decision.id;
        trade.volume = decision.amount;
        trade.price = decision.price;
        trade.type = decision.type;
        trade.buy_order_id = 0;
        trade.sell_order_id = 0;
        return {trade};
    }

private:

    double _commission;
};


#endif // CTRADING__BROKERS__PRETENDBROKER__HPP
