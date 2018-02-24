#include "history/DBHistory.hpp"
#include "history/MemoryHistory.hpp"
#include "sources/CSVSource.hpp"


int main(int argc, char const *argv[]) {

    DBHistory db_history("/tmp/cpptrading-tests/history_plot");
    // {
    //     CSVSource source("btceur", "data/localbtcEUR.csv");
    //     source.historize(db_history);
    //     source.parse();
    // }

    MemoryHistory mem_history;
    mem_history.synchronize_with(db_history);
    mem_history.plot();
    return 0;
}
