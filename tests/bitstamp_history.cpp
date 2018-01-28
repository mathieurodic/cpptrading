#include "history/BitstampHistory.hpp"


int main(int argc, char const *argv[]) {
    BitstampHistory history("/tmp/cpptrading-tests/BitstampHistory", "btceur");
    getchar();
    return 0;
}
