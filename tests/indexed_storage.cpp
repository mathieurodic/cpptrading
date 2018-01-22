#include "db/IndexedStorage.hpp"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <array>


#pragma pack(push, 1)
struct item_t {
    inline item_t() {}
    inline item_t(int64_t i) {
        val8 = i;
        val64 = i;
        memset(&str20, 0, sizeof(str20));
        snprintf(str20.data(), sizeof(str20), "%ld", i);
        memset(&fx_str20, 0, sizeof(fx_str20));
        snprintf(fx_str20.data(), sizeof(fx_str20), "%19ld", i);
    }
    inline const bool operator==(const item_t& other) const {
        return memcmp(this, &other, sizeof(*this)) == 0;
    }
    inline const bool operator<(const item_t& other) const {
        return val64 < other.val64;
    }
    std::array<char, 20> str20;
    std::array<char, 20> fx_str20;
    uint8_t val8;
    int64_t val64;
};
#pragma pack(pop)

inline std::ostream& operator << (std::ostream& os, const item_t& item) {
    return os << "<Item" << item.val64 << '>';
}

template <size_t size> inline std::ostream& operator << (std::ostream& os, const std::array<char, size>& char_array) {
    return os << char_array.data();
}

static const std::string path = "db/test-is";


void reset_files() {
    std::remove((path + ".index.val8").c_str());
    std::remove((path + ".index.val64").c_str());
    std::remove((path + ".index.fx_str20").c_str());
    std::remove((path + ".log.gz").c_str());
}


int main(int argc, char const *argv[]) {
    // reset_files();
    // {
    //     std::cout << "\nTEST WITH ONE KEY INDEX: val64" << '\n';
    //     INDEXED_STORAGE(item_t, val64) storage(path, false, 1<<24, false);
    //     for (int i=-12345; i<12345; ++i) {
    //         item_t item = i;
    //         storage.insert(item);
    //     }
    //     std::cout << "inserted values" << '\n';
    //     std::cout << "is_consistent = " << storage.integrity_check() << '\n';
    // }
    reset_files();
    {
        std::cout << "\nTEST WITH ONE KEY INDEX: fx_str20" << '\n';
        INDEXED_STORAGE(item_t, fx_str20) storage(path, false, 1<<24, false);
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            storage.insert(item);
        }
        std::cout << "inserted values" << '\n';
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            if (storage.count_fx_str20(item) != 1) {
                std::cout << item << " : " << storage.count_fx_str20(item) << '\n';
            }
        }
        std::cout << "checked values" << '\n';
        std::cout << "is_consistent = " << storage.integrity_check() << '\n';
        item_t item(-12345);
        std::cout << storage.count_fx_str20(item) << '\n';
    }
    reset_files();
    {
        std::cout << "\nTEST WITH ONE KEY INDEX: val8" << '\n';
        INDEXED_STORAGE(item_t, val8) storage(path, true);
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            storage.insert(item);
        }
        std::cout << "inserted values" << '\n';
        std::cout << "is_consistent = " << storage.integrity_check() << '\n';
    }
    reset_files();
    {
        std::cout << "\nTEST WITH TWO KEY INDINCES: val8, val64" << '\n';
        INDEXED_STORAGE_2(item_t, val8, val64) storage(path, true, false);
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            storage.insert(item);
        }
        std::cout << "inserted values" << '\n';
        std::cout << "is_consistent = " << storage.integrity_check() << '\n';
    }
    reset_files();
    {
        std::cout << "\nTEST WITH TWO KEY INDINCES: fx_str20, val8" << '\n';
        INDEXED_STORAGE_2(item_t, fx_str20, val8) storage(path, true, true);
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            storage.insert(item);
        }
        std::cout << "inserted values" << '\n';
        std::cout << "is_consistent = " << storage.integrity_check() << '\n';
    }
    reset_files();
    return 0;
}
