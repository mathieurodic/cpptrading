#include "db/UpscaleBTree.hpp"

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
    // auto range = btree.get();
    // for (auto it=range.begin(); it!=range.end(); ++it) {
    //     std::cout << *it << '\n';
    // }
    for (const auto& test : btree.get_range(9, 12)) {
        std::cout << test << '\n';
    }

    printf("\nCount records & keys\n");
    {
        std::vector<float> values = {-4, -6, 87, 21, 1, 0, 3, 999, 1<<18};
        for (auto value : values) {
            test_t test = {
                .a = value,
                .b = value,
                .c = value,
                .d = value,
            };
            std::cout << value << ": " << btree.count(test) << ", " << btree.count(value) << ", " << (btree.contains(value) ? "true" : "false") << '\n';
        }
    }

    printf("\nRetrieving non-existing keys\n");
    std::cout << btree.get_ge(-1.0) << '\n';
    std::cout << btree.get_le(1e9) << '\n';
    try {
        std::cout << btree.get(-1.0) << '\n';
    } catch (const DBKeyException& exception) {
        std::cout << "could not retrieve key '-1'" << '\n';
    }

    printf("\nTry with duplicates\n");
    {
        std::string btree_path = "/tmp/ctrading/test/btree.dup.btree";
        UPSCALE_BTREE(test_t, c) btree(btree_path, true);
        for (int i=0; i<32; ++i) {
            test_t test = {
                .a = i,
                .b = i,
                .c = i,
                .d = i,
            };
            btree.insert(test);
            if (i < 16) {
                btree.insert(test);
            }
        }
        for (const auto& test : btree.get()) {
            std::cout << test << '\n';
        }
    }


    return 0;
}
