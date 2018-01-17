#include "db/GzipLog.hpp"

#include <iostream>
#include <cstdio>


int main(int argc, char const *argv[]) {
    const std::string path = "db/test-gziplog.gz";
    std::remove(path.c_str());
    {
        std::cout << "STORE" << '\n';
        size_t size = 0;
        GzipLogWriter writer(path);
        for (size_t i = 100000; i < 1000000; i++) {
            const std::string item = std::to_string(i);
            writer.append(item);
            size += item.size();
        }
        std::cout << "STORED " << size << " BYTES\n";
    }
    {
        std::cout << "READ" << '\n';
        typedef char char4_t[6];
        GzipLogReader reader(path);
        for (size_t i = 100000; i < 1000000; i++) {
            char4_t item_data;
            reader.next<char4_t>(item_data);
            const std::string item((const char*) &item_data, sizeof(item_data));
            const std::string expected_item = std::to_string(i);
            if (item != expected_item) {
                std::cout << "GOT " << item << ", EXPECTED " << expected_item << '\n';
            }
        }
    }
    return 0;
}
