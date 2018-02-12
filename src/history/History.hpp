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

    virtual TradeAverage get_average(const double& timestamp_begin, const double timestamp_end) {
        return TradeAverage();
    }

    template <typename T>
    inline Range<T> get() {
        return Range<T>();
    }

    virtual Range<Trade> get_trades() = 0;
    virtual Range<Order> get_orders() = 0;
    virtual Range<Decision> get_decisions() = 0;

    virtual Span get_time_span() {
        return {
            .min = NAN,
            .max = NAN,
        };
    }

    template <typename T>
    inline const std::pair<size_t, size_t> synchronize_with(History& history_2) {
        History& history_1 = *this;
        std::unordered_set<T> specific_instances_1;
        for (const T& instance : history_1.template get<T>()) {
            // std::cout << "THIS: " << instance << '\n';
            specific_instances_1.insert(instance);
        }
        //
        std::unordered_set<T> specific_instances_2;
        for (const T& instance : history_2.template get<T>()) {
            // std::cout << "OTHER: " << instance << '\n';
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


template <typename T, typename History1, typename History2,
    typename std::enable_if<std::is_base_of<History, History1>::value, History>::type* = nullptr,
    typename std::enable_if<std::is_base_of<History, History2>::value, History>::type* = nullptr>
inline std::pair<size_t, size_t> synchronize(History1& history_1, History2& history_2) {
    std::unordered_set<T> specific_instances_1;
    for (const T& instance : history_1.template get<T>()) {
        // std::cout << "THIS: " << instance << '\n';
        specific_instances_1.insert(instance);
    }
    //
    std::unordered_set<T> specific_instances_2;
    for (const T& instance : history_2.template get<T>()) {
        // std::cout << "OTHER: " << instance << '\n';
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
