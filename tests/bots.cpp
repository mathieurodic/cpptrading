#include <iostream>

#include "bots/RandomBot.hpp"
#include "brokers/PretendBroker.hpp"
#include "sources/CSVSource.hpp"
#include "history/DBHistory.hpp"
#include "history/LogHistory.hpp"
#include "history/MemoryHistory.hpp"


static const bool must_parse = true;


int main(int argc, char const *argv[]) {
    MemoryHistory mem_history;
    LogHistory log_history;
    //
    std::cout << "\nLOADED HISTORIES\n\n";
    if (must_parse) {
        CSVSource source("btceur", "data/localbtcEUR.csv");
        std::cout << "\nSOURCED\n\n";
        source.historize(mem_history);
        source.parse(1000);
        std::cout << "\nPARSED\n\n";
    }
    {
        PretendBroker broker(100, 2.5e-3);
        broker.historize(mem_history);
        broker.historize(log_history);
        RandomBot bot(mem_history, broker);
        std::cout << "FINISHED BROKER & BOT" << '\n';
        auto span = mem_history.get_time_span();
        std::cout << "SIMULATING " << bot.get_name() << " FROM " << Timestamp(span.min) << " TO " << Timestamp(span.max) << '\n';
        for (double t=span.min; t<=span.max; t+= 60.) {
            bot.decide_and_execute(t);
            std::cout << broker << '\n';
        }
        std::cout << "TESTED\n";
    }
    return 0;
}
