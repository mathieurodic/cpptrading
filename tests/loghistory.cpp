#include "history/LogHistory.hpp"


inline void test(LogHistory& history) {
    Order order;
    history.record(order);
    memset(&order, 0, sizeof(order));
    history.record(order);
    //
    Trade trade;
    history.record(trade);
    memset(&trade, 0, sizeof(trade));
    history.record(trade);
}


int main(int argc, char const *argv[]) {
    {
        LogHistory history("/tmp/cpptrading-tests/LogHistory.log");
        test(history);
    }
    {
        LogHistory history;
        test(history);
    }

    return 0;
}
