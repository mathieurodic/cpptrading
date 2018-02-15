#ifndef CTRADING__HISTORY__HISTORY__HPP
#define CTRADING__HISTORY__HISTORY__HPP


#include "models/Timestamp.hpp"
#include "models/Trade.hpp"
#include "models/TradeSummary.hpp"
#include "models/Order.hpp"
#include "models/Decision.hpp"

#include "range/Range.hpp"
#include "math/Plotter.hpp"

#include <set>
#include <ostream>


struct SynchronizationResult {
    std::pair<size_t, size_t> trades;
    std::pair<size_t, size_t> orders;
    std::pair<size_t, size_t> decisions;
};

inline std::ostream& operator<<(std::ostream& os, const SynchronizationResult result) {
    return (os
        << "<SynchronizationResult trades:(" << result.trades.first << ", " << result.trades.second << ")"
        << " orders:(" << result.orders.first << ", " << result.orders.second << ")"
        << " decisions:(" << result.decisions.first << ", " << result.decisions.second << ")"
        << ">"
    );
}


class History {
public:

    virtual void feed(Trade& trade) = 0;
    virtual void feed(Order& order) = 0;
    virtual void feed(Decision& decision) = 0;

    template <typename T>
    inline Range<T> get() {
        return Range<T>();
    }

    virtual Range<Trade> get_trades() = 0;
    virtual Range<Trade> get_trades_by_timestamp(Timestamp timestamp_begin, Timestamp timestamp_end) {
        return get_trades().filter([timestamp_begin, timestamp_end] (const Trade& trade) -> bool {
            return trade.timestamp > timestamp_begin && trade.timestamp <= timestamp_end;
        });
    }
    virtual TradeSummary get_trade_summary(const double& timestamp_begin, const double timestamp_end) {
        TradeSummary summary;
        for (const Trade& trade : get_trades_by_timestamp(timestamp_begin, timestamp_end)) {
            summary += trade;
        }
        return summary;
    }

    virtual Range<Order> get_orders() = 0;
    virtual Range<Decision> get_decisions() = 0;

    virtual void plot() {
        Plotter plotter;
        TimestampSpan span = get_time_span();
        plotter.axes.x.type = PlotterAxisParameters::TEMPORAL;
        plotter.axes.x.min = span.to - 365.25 * 24. * 3600.;
        plotter.axes.x.max = span.to;
        plotter.axes.x.origin = 0.;
        plotter.axes.x.grid = 365.25 * 24. * 3600. / 12.;
        //
        plotter.axes.y.type = PlotterAxisParameters::LOGARITHMIC;
        plotter.axes.y.min = 1.;
        plotter.axes.y.max = 100000.;
        plotter.axes.y.origin = 1.;
        plotter.axes.y.grid = 10.;
        //
        History* history = this;
        plotter.plot([history](double t1, double t2) mutable {
            return history->get_trade_summary(t1, t2).average_price;
            // const double delta = t2 - t1;
            // return std::pair<double, double>(
            //     history->get_trade_summary(t1 - delta/2., t1 + delta/2.).average_price,
            //     history->get_trade_summary(t2 - delta/2., t2 + delta/2.).average_price
            // );
        }, GREEN);
        plotter.start();
    }

    virtual TimestampSpan get_time_span() {
        return {};
    }

    template <typename T>
    inline const std::pair<size_t, size_t> synchronize_with(History& history_2) {
        History& history_1 = *this;
        std::set<T> specific_instances_1;
        for (const T& instance : history_1.template get<T>()) {
            specific_instances_1.insert(instance);
        }
        //
        std::set<T> specific_instances_2;
        for (const T& instance : history_2.template get<T>()) {
            auto it = specific_instances_1.find(instance);
            if (it == specific_instances_1.end()) {
                specific_instances_2.insert(instance);
            } else {
                specific_instances_1.erase(it);
            }
        }
        //
        for (T instance : specific_instances_1) {
            history_2.feed(instance);
        }
        for (T instance : specific_instances_2) {
            history_1.feed(instance);
        }
        //
        return {specific_instances_1.size(), specific_instances_2.size()};
    }
    inline const SynchronizationResult synchronize_with(History& other) {
        SynchronizationResult result;
        result.trades = synchronize_with<Trade>(other);
        result.orders = synchronize_with<Order>(other);
        result.decisions = synchronize_with<Decision>(other);
        return result;
    }

};



template <>
Range<Trade> History::get() {
    return get_trades();
}
template <>
Range<Order> History::get() {
    return get_orders();
}
template <>
Range<Decision> History::get() {
    return get_decisions();
}


#endif // CTRADING__HISTORY__HISTORY__HPP
