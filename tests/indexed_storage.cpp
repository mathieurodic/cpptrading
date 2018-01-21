#include "db/IndexedStorage.hpp"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


#pragma pack(push, 1)
struct item_t {
    inline item_t() {}
    inline item_t(int64_t i) {
        val8 = i;
        val64 = i;
        snprintf(str20, sizeof(str20), "%ld", i);
        snprintf(fx_str20, sizeof(fx_str20), "%20ld", i);
    }
    inline const bool operator==(const item_t& other) const {
        return memcmp(this, &other, sizeof(*this)) == 0;
    }
    char str20[20];
    char fx_str20[20];
    uint8_t val8;
    int64_t val64;
};
#pragma pack(pop)


static const std::string path = "db/test-is";


void reset_files() {
    std::remove((path + ".val64.index").c_str());
    std::remove((path + ".str20.index").c_str());
    std::remove((path + ".log").c_str());
}


int main(int argc, char const *argv[]) {
    reset_files();
    {
        std::cout << "\nTEST WITH ONE KEY INDEX" << '\n';
        INDEXED_STORAGE(item_t, val64) storage(path, false, 1<<24, false);
        for (int i=-123456; i<123456; ++i) {
            item_t item = i;
            storage.insert(item);
        }
    }
    reset_files();
    {
        std::cout << "\nTEST WITH TWO KEYS INDEX" << '\n';
        INDEXED_STORAGE_2(item_t, val64, str20) storage(path);
        for (int i=-123456; i<123456; ++i) {
            item_t item = i;
            storage.insert(item);
        }
    }
    return 0;
}
