#include <iostream>

#include "sources/CSVSource.hpp"
#include "history/DBHistory.hpp"
#include "history/LogHistory.hpp"
#include "history/MemoryHistory.hpp"


int main(int argc, char const *argv[]) {
    {
        DBHistory db_history("/tmp/cpptrading-tests/history_sync_db");
        MemoryHistory mem_history;
        CSVSource source("btceur", "data/localbtcEUR.csv");
        source.historize(db_history);
        source.parse(10);

        std::cout << "FINISHED PARSING" << '\n';
        for (const Trade& trade : db_history.get<Trade>()) {
            std::cout << trade << '\n';
        }
        std::cout << "SHOWED TRADES FROM DB" << '\n';
        std::cout << mem_history.synchronize_with(db_history) << '\n';
        std::cout << "SYNCHRONIZED DB & MEMORY" << '\n';
        for (const Trade& trade : mem_history.get<Trade>()) {
            std::cout << trade << '\n';
        }
        std::cout << "SHOWED TRADES FROM MEMORY" << '\n';
        std::cout << mem_history.synchronize_with(db_history) << '\n';
        std::cout << "SYNCHRONIZED DB & MEMORY" << '\n';
        for (const Trade& trade : mem_history.get<Trade>()) {
            std::cout << trade << '\n';
        }
        std::cout << "SHOWED TRADES FROM MEMORY" << '\n';
    }
    std::cout << "\n\n\n\n";
    {
        DBHistory db_history("/tmp/cpptrading-tests/history_sync_db_2");
        CSVSource source("btceur", "data/localbtcEUR.csv");
        source.historize(db_history);
        source.parse();
        std::cout << "PARSED" << '\n';
    }
    std::cout << "\n\n\n\n";
    {
        DBHistory db_history("/tmp/cpptrading-tests/history_sync_db_2");
        MemoryHistory mem_history_1;
        MemoryHistory mem_history_2;
        std::cout << mem_history_1.synchronize_with(db_history) << '\n';
        std::cout << "SYNCHRONIZED MEMORY1 <-> DB" << '\n';
        std::cout << mem_history_1.synchronize_with(db_history) << '\n';
        std::cout << "SYNCHRONIZED MEMORY1 <-> MEMORY2" << '\n';
        std::cout << mem_history_1.synchronize_with(mem_history_2) << '\n';
        std::cout << "SYNCHRONIZED MEMORY1 <-> MEMORY2" << '\n';
    }
    return 0;
}
