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
        source.parse();
        std::cout << "\nPARSED\n\n";
    }
    {
        PretendBroker broker;
        broker.historize(mem_history);
        // broker.historize(log_history);
        broker.init(Timestamp(2001,1,1), 100, 2.5e-3);
        RandomBot bot(mem_history, broker, 1e-3);
        std::cout << "FINISHED BROKER & BOT" << '\n';
        auto span = mem_history.get_time_span();
        std::cout << "SIMULATING '" << bot.get_name() << "': " << span << '\n';
        for (double t=span.from; t<=span.to; t+= 60.) {
            bot.decide_and_execute(t);
            // std::cout << broker << '\n';
        }
        std::cout << "TESTED\n";
    }
    mem_history.plot();
    return 0;
}
