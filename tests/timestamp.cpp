#include <iostream>

#include "models/Timestamp.hpp"


int main(int argc, char const *argv[]) {
    Timestamp now;
    std::cout << now << '\n';
    std::cout << (now + 86400) << '\n';
    std::cout << (now - 666) << '\n';
    std::cout << Timestamp(1985, 9, 12) << '\n';
    std::cout << Timestamp(1983, 12, 8) << '\n';
    std::cout << Timestamp(2016, 12, 23) << '\n';
    return 0;
}
