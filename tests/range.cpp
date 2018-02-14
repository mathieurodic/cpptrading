#include <iostream>

#include "range/SortedRange.hpp"
#include "range/ForwardRange.hpp"

#include <map>
#include <list>


#define Associative std::multimap
#define Forward std::list


int main(int argc, char *argv[]) {
    Associative<std::string, int> map;
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

    std::cout << "\n\nTESTED ASSOCIATIVE\n\n" << '\n';


    Forward<std::string> forward;
    for (int i=1001; i<=1111; ++i) {
        forward.push_back(std::to_string(i));
    }
    for (const std::string& s : ForwardRangeFactory(forward)) {
        std::cout << s << '\n';
    }
    std::cout << "\nShowed full range\n\n";
    std::cout << "\n\nTESTED FORWARD\n\n" << '\n';
    return 0;
}
