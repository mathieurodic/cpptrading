#include "sources/BitstampSource.hpp"
#include "history/LogHistory.hpp"
#include "history/DBHistory.hpp"


int main(int argc, char const *argv[]) {
    BitstampSource source("btceur");
    LogHistory log_history;
    LogHistory txt_history("/tmp/cpptrading-tests/bitstamp_source.log");
    DBHistory db_history("/tmp/cpptrading-tests/bitstamp_source_db");
    source.historize(log_history);
    source.historize(txt_history);
    source.historize(db_history);
    getchar();
    return 0;
}
