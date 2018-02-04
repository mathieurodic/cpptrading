#include "sources/BitstampSource.hpp"
#include "history/LogHistory.hpp"


int main(int argc, char const *argv[]) {
    BitstampSource source("btceur");
    LogHistory history;
    source.historize(history);
    getchar();
    return 0;
}
