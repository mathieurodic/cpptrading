#include "history/DBHistory.hpp"
#include "history/MemoryHistory.hpp"
#include "sources/CSVSource.hpp"


void analyze(History& history, const std::string& name) {
    std::cout << '\n';
    std::cout << history.get_trade_summary(Timestamp(2018, 1, 1), Timestamp(2018, 1, 2)) << '\n';
    std::cout << history.get_time_span() << '\n';
    std::cout << "Analyzed " << name << '\n';
}


int main(int argc, char const *argv[]) {

    DBHistory db_history("/tmp/cpptrading-tests/history_analysis");
    MemoryHistory mem_history;
    // CSVSource source("btceur", "data/localbtcEUR.csv");
    // source.historize(mem_history);
    // source.parse();
    // std::cout << "PARSED CSV SOURCE" << '\n';
    // std::cout << mem_history.get_trades() << '\n';

    std::cout << mem_history.synchronize_with(db_history) << '\n';
    std::cout << "SYNCHRONIZED MEM & DB" << '\n';

    analyze(mem_history, "mem_history");
    analyze(db_history, "db_history");

    return 0;
}
