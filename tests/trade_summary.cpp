#include "history/DBHistory.hpp"
#include "history/MemoryHistory.hpp"
#include "sources/CSVSource.hpp"


int main(int argc, char const *argv[]) {

    DBHistory db_history("/tmp/cpptrading-tests/trade_summary");
    MemoryHistory mem_history;
    // CSVSource source("btceur", "data/localbtcEUR.csv");
    // source.historize(mem_history);
    // source.parse();
    // std::cout << "PARSED CSV SOURCE" << '\n';

    std::cout << mem_history.synchronize_with(db_history) << '\n';
    std::cout << "SYNCHRONIZED MEM & DB" << '\n';

    std::cout << mem_history.get_trade_summary(Timestamp(2018, 1, 1), Timestamp(2018, 1, 2)) << '\n';
    std::cout << db_history.get_trade_summary(Timestamp(2018, 1, 1), Timestamp(2018, 1, 2)) << '\n';

    return 0;
}
