#include "history/LogHistory.hpp"
#include "sources/CSVSource.hpp"


int main(int argc, char const *argv[]) {
    {
        LogHistory debug_history;
        LogHistory log_history("/tmp/cpptrading-tests/log_history");
        CSVSource source("btceur", "data/localbtcEUR.csv");
        source.historize(log_history);
        source.historize(debug_history);
        source.parse(20);
        std::cout << "PARSED" << '\n';
        std::cout << synchronize(log_history, debug_history) << '\n';
    }
    return 0;
}
