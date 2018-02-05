#include <iostream>

#include "sources/CSVSource.hpp"
#include "history/LogHistory.hpp"
#include "history/MemoryHistory.hpp"
#include "history/DBHistory.hpp"


int main(int argc, char const *argv[]) {
    LogHistory log_history;
    MemoryHistory mem_history;
    DBHistory db_history("db/CSVSourceDB");
    CSVSource source("btceur", "data/localbtcEUR.csv");
    source.historize(log_history);
    source.historize(mem_history);
    source.historize(db_history);
    source.parse();
    return 0;
}
