#include "db/BTree.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <string.h>


#pragma pack(push, 1)

struct test_t {
    int16_t a;
    char b;
    float c;
    size_t d;
    inline const bool operator!=(const test_t& other) const {
        return memcmp(this, &other, sizeof(*this));
    }
    inline const bool operator==(const test_t& other) const {
        return !memcmp(this, &other, sizeof(*this));
    }
};

#pragma pack(pop)


std::ostream& operator<<(std::ostream& os, test_t test) {
    os << test.a << ", " << (int) test.b << ", " << test.c << ", " << test.d;
}


int main(int argc, char const *argv[]) {
    std::string btree_path = "/tmp/ctrading/test/btree.btree";
    UPSCALE_BTREE(test_t, c) btree(btree_path);

    printf("\nInstanciating tests\n");
    std::vector<test_t> tests;
    for (int i=0; i<(1<<17); ++i) {
        tests.push_back({
            .a = i,
            .b = i,
            .c = i,
            .d = i,
        });
    }

    printf("\nInserting\n");
    for (test_t& test : tests) {
        btree.insert(test);
    }

    printf("\nRetrieving points\n");
    for (test_t& test : tests) {
        const test_t t = btree.get(test.c);
        if (t != test) {
            std::cout << "MISMATCH" << '\n';
        }
    }

    printf("\nRetrieving range\n");
    // auto range = btree.get_all();
    // for (auto it=range.begin(); it!=range.end(); ++it) {
    //     std::cout << *it << '\n';
    // }
    for (const auto& test : btree.get_range(9, 12)) {
        std::cout << test << '\n';
    }

    printf("\nRetrieving non-existing keys\n");
    std::cout << btree.get_ge(-1.0) << '\n';
    std::cout << btree.get_le(1e9) << '\n';
    std::cout << btree.get(-1.0) << '\n';

    return 0;
}
