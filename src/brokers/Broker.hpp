#ifndef CTRADING__BROKERS__BROKER__HPP
#define CTRADING__BROKERS__BROKER__HPP


#include <set>
#include <vector>


class Broker {
public:

    virtual const std::string get_name() const {
        return "Broker";
    }

    inline void historize(History& history) {
        _histories.insert(&history);
    }

    virtual std::vector<Trade> send(Decision& decision) = 0;

    const bool validate(Decision& decision) {
        if (decision.type == WAIT) {
            return false;
        }
        if (std::isnan(decision.price)) {
            return false;
        }
        const Balance balance = get_balance_at_timestamp(decision.timestamp);
        if (std::isnan(decision.amount)) {
            switch (decision.type) {
                case BUY:
                    decision.amount = (balance.liquidity - balance.commission) / decision.price;
                    break;
                case SELL:
                    decision.amount = balance.stock / decision.price;
                    break;
                case WAIT:
                    decision.amount = 0.;
                    break;
            }
        }
        if (balance.liquidity < 0. || decision.amount < 0 || balance.liquidity + balance.stock * decision.price < balance.commission) {
            return false;
        }
        if (decision.amount == 0.) {
            return false;
        }
        return true;
    }

    template <typename T>
    void feed(T& item) {
        for (auto history : _histories) {
            history->feed(item);
        }
    }

    virtual const double compute_commission(const Trade& trade) = 0;

    const Balance get_balance_at_timestamp(const Timestamp& timestamp) {
        for (History* history : _histories) {
            return history->get_balance_at_timestamp(timestamp);
        }
        return Balance();
    }

    void execute(Decision& decision) {
        Decision original_decision = decision;
        if (decision.type == WAIT) {
            return;
        }
        std::vector<Trade> trades;
        if (validate(decision)) {
            trades = send(decision);
        } else {
            decision.status = Decision::CANCELLED;
        }
        //
        for (Trade& trade : trades) {
            BalanceChange balance_change;
            balance_change.timestamp = trade.timestamp;
            balance_change.origin.type = BalanceChangeOrigin::TRADE;
            balance_change.origin.id = trade.id;
            switch (decision.type) {
                case BUY:
                    balance_change.delta.liquidity = -trade.volume * trade.price;
                    balance_change.delta.stock = +trade.volume;
                    break;
                case SELL:
                    balance_change.delta.liquidity = +trade.volume * trade.price;
                    balance_change.delta.stock = -trade.volume;
                    break;
                case WAIT:
                    break;
            }
            balance_change.delta.commission = compute_commission(trade);
            balance_change.consolidated = get_balance_at_timestamp(trade.timestamp) + balance_change.delta;
            // std::cout << balance_change << '\n';
            feed(trade);
            feed(balance_change);
        }
        BalanceChange balance_change;
        //
        if (original_decision != decision) {
            feed(original_decision);
        }
        feed(decision);
    }

protected:

    std::set<History*> _histories;

};


#include <ostream>

inline std::ostream& operator<<(std::ostream& os, Broker& broker) {
    return (os
        << "<" << broker.get_name()
        << ">"
    );
}


#endif // CTRADING__BROKERS__BROKER__HPP
