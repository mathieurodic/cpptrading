#ifndef CTRADING__HISTORY__HISTORY__HPP
#define CTRADING__HISTORY__HISTORY__HPP


#include "models/Trade.hpp"
#include "models/TradeAverage.hpp"
#include "models/Order.hpp"
#include "models/Decision.hpp"

#include "iteration.hpp"

#include <unordered_set>
#include <ostream>


struct Span {
    double min;
    double max;
    void operator += (const double& value) {
        min += value;
        max += value;
    }
    void operator -= (const double& value) {
        min -= value;
        max -= value;
    }
};


class History {
public:

    virtual void feed(Trade& trade) = 0;
    virtual void feed(Order& order) = 0;
    virtual void feed(Decision& decision) = 0;

    virtual TradeAverage get_average(const double& timestamp_begin, const double timestamp_end) {
        return TradeAverage();
    }

    template <typename Model>
    Range<Model> get_all() {
        return Range<Model>();
    }

    virtual Span get_time_span() {
        return {
            .min = NAN,
            .max = NAN,
        };
    }

};


template <typename Model, typename History1, typename History2,
    typename std::enable_if<std::is_base_of<History, History1>::value, History>::type* = nullptr,
    typename std::enable_if<std::is_base_of<History, History2>::value, History>::type* = nullptr>
inline std::pair<size_t, size_t> synchronize(History1& history_1, History2& history_2) {
    std::unordered_set<Model> specific_instances_1;
    for (const Model& instance : history_1.template get_all<Model>()) {
        // std::cout << "THIS: " << instance << '\n';
        specific_instances_1.insert(instance);
    }
    //
    std::unordered_set<Model> specific_instances_2;
    for (const Model& instance : history_2.template get_all<Model>()) {
        // std::cout << "OTHER: " << instance << '\n';
        auto it = specific_instances_1.find(instance);
        if (it == specific_instances_1.end()) {
            specific_instances_2.insert(instance);
        } else {
            specific_instances_1.erase(it);
        }
    }
    //
    for (Model instance : specific_instances_1) {
        history_2.feed(instance);
    }
    for (Model instance : specific_instances_2) {
        history_1.feed(instance);
    }
    //
    return {specific_instances_1.size(), specific_instances_2.size()};
}


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


template <typename History1, typename History2,
    typename std::enable_if<std::is_base_of<History, History1>::value, History>::type* = nullptr,
    typename std::enable_if<std::is_base_of<History, History2>::value, History>::type* = nullptr>
inline const SynchronizationResult synchronize(History1& history_1, History2& history_2) {
    SynchronizationResult result;
    result.trades = synchronize<Trade>(history_1, history_2);
    result.orders = synchronize<Order>(history_1, history_2);
    result.decisions = synchronize<Decision>(history_1, history_2);
    return result;
}


#endif // CTRADING__HISTORY__HISTORY__HPP
