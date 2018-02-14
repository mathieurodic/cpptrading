#include "range/SortedRange.hpp"


#include <iostream>
#include <map>


#define map_type std::multimap


int main(int argc, char *argv[]) {
    map_type<std::string, int> map;
    for (int i=0; i<100; ++i) {
        map.insert({std::to_string(i), i});
    }
    for (const int& i : SortedRangeFactory(map)) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed full range\n\n";
    for (const int& i : SortedRangeFactory(map, "1", "2")) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed limited range (1-2)\n\n";
    auto range = SortedRangeFactory(map, "1", "2");
    auto range_copy = range;
    for (const int& i : range_copy) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed limited range from copy (1-2)\n\n";
    auto filtered_range = range.filter([] (const int& input) -> bool {
        return input % 2 == 0;
    });
    for (const int& i : filtered_range) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed filtered range from copy (even)\n\n";
    filtered_range = range.filter([] (const int& input) -> bool {
        return input % 2 == 0;
    });
    auto filtered_range_2 = filtered_range.filter([] (const int& input) -> bool {
        return input % 3 == 0;
    });
    for (const int& i : filtered_range_2) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed filtered range from copy (even, divisible by 3)\n\n";
    filtered_range = range.filter([] (const int& input) -> bool {
        return input % 2 == 0;
    }).filter([] (const int& input) -> bool {
        return input % 3 == 0;
    });
    for (const int& i : filtered_range_2) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed chained ranges (even, divisible by 3)\n\n";
    return 0;
}
