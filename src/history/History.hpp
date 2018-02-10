#ifndef CTRADING__HISTORY__HISTORY__HPP
#define CTRADING__HISTORY__HISTORY__HPP


#include "models/Trade.hpp"
#include "models/TradeAverage.hpp"
#include "models/Order.hpp"
#include "models/Decision.hpp"

#include "iteration.hpp"

#include <unordered_set>


class History {
public:

    virtual void feed(Trade& trade) = 0;
    virtual void feed(Order& order) = 0;
    virtual void feed(Decision& decision) = 0;

    virtual TradeAverage get_average(const double& timestamp_begin, const double timestamp_end) {
        return TradeAverage();
    }

};


template <typename Model, typename History1, typename History2,
    typename std::enable_if<std::is_base_of<History, History1>::value, History>::type* = nullptr,
    typename std::enable_if<std::is_base_of<History, History2>::value, History>::type* = nullptr>
inline void synchronize(History1& history_1, History2& history_2) {
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
    std::cout << "history_1 -> history_2: " << specific_instances_1.size() << '\n';
    for (Model instance : specific_instances_1) {
        history_2.feed(instance);
    }
    std::cout << "history_2 -> history_1: " << specific_instances_2.size() << '\n';
    for (Model instance : specific_instances_2) {
        history_1.feed(instance);
    }
}

template <typename History1, typename History2,
    typename std::enable_if<std::is_base_of<History, History1>::value, History>::type* = nullptr,
    typename std::enable_if<std::is_base_of<History, History2>::value, History>::type* = nullptr>
inline void synchronize(History1& history_1, History2& history_2) {
    synchronize<Trade>(history_1, history_2);
    synchronize<Order>(history_1, history_2);
    synchronize<Decision>(history_1, history_2);
}


#endif // CTRADING__HISTORY__HISTORY__HPP
