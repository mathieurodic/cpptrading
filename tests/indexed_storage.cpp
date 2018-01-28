#include "db/RotatingLog.hpp"
#include "db/UpscaleBTree.hpp"
#include "db/IndexedStorage.hpp"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <array>

#include "./item.hpp"


static const std::string path = "/tmp/cpptrading-tests/IndexedStorage";


void reset_files() {
    std::experimental::filesystem::path b = path;
    for (auto& p : std::experimental::filesystem::directory_iterator(b.parent_path())) {
        const std::string filepath = p.path().string();
        std::cout << filepath << '\n';
    }
    // std::remove((path + ".index.integer_number").c_str());
    // std::remove((path + ".index.floating_number").c_str());
    // std::remove((path + ".index.text").c_str());
    // std::remove((path + ".log.gz").c_str());
}


int main(int argc, char const *argv[]) {
    // reset_files();
    // {
    //     std::cout << "\nTEST WITH ONE KEY INDEX: floating_number" << '\n';
    //     INDEXED_STORAGE(item_t, floating_number) storage(path, false, 1<<24, false);
    //     for (int i=-12345; i<12345; ++i) {
    //         item_t item = i;
    //         storage.insert(item);
    //     }
    //     std::cout << "inserted values" << '\n';
    //     std::cout << "is_consistent = " << storage.integrity_check() << '\n';
    // }
    reset_files();
    {
        std::cout << "\nTEST WITH ONE KEY INDEX: text" << '\n';
        INDEXED_STORAGE(item_t, text, RotatingLog, UpscaleBTree) storage(path);
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            storage.insert(item);
        }
        std::cout << "inserted values" << '\n';
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            if (storage.count_text(item) != 1) {
                std::cout << item << " : " << storage.count_text(item) << '\n';
            }
        }
        std::cout << "checked values" << '\n';
        std::cout << "is_consistent = " << storage.integrity_check() << '\n';
        item_t item(-12345);
        std::cout << storage.count_text(item) << '\n';
    }
    reset_files();
    {
        std::cout << "\nTEST WITH ONE KEY INDEX: integer_number" << '\n';
        INDEXED_STORAGE(item_t, integer_number, RotatingLog, UpscaleBTree) storage(path);
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            storage.insert(item);
        }
        std::cout << "inserted values" << '\n';
        std::cout << "is_consistent = " << storage.integrity_check() << '\n';
    }
    reset_files();
    {
        std::cout << "\nTEST WITH TWO KEY INDINCES: integer_number, floating_number" << '\n';
        INDEXED_STORAGE_2(item_t, integer_number, floating_number, RotatingLog, UpscaleBTree) storage(path);
        for (int i=-12345; i<12345; ++i) {
            item_t item = i;
            storage.insert(item);
        }
        std::cout << "inserted values" << '\n';
        std::cout << "is_consistent = " << storage.integrity_check() << '\n';
    }
    reset_files();
    {
        std::cout << "\nTEST WITH TWO KEY INDINCES: text, integer_number" << '\n';
        INDEXED_STORAGE_2(item_t, text, integer_number, RotatingLog, UpscaleBTree) storage(path);
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
