#include "db/GzipLog.hpp"

#include <iostream>
#include <cstdio>


int main(int argc, char const *argv[]) {
    const std::string path = "db/test-gziplog.gz";
    std::remove(path.c_str());
    {
        std::cout << "STORE...\n";
        size_t size = 0;
        GzipLogWriter writer(path);
        for (size_t i = 100000; i < 1000000; i++) {
            const std::string item = std::to_string(i);
            writer.append(item);
            size += item.size();
        }
        std::cout << "STORED " << size << " BYTES\n";
    }
    for (int _=0;_<4;++_) {
        std::cout << "CHECK...\n";
        typedef char char6_t[6];
        size_t size = 0;
        GzipLogReader reader(path);
        for (size_t i = 100000; i < 1000000; i++) {
            size += sizeof(char6_t);
            char6_t item_data;
            auto result = reader.next<char6_t>(item_data);
            const std::string item((const char*) &item_data, sizeof(item_data));
            const std::string expected_item = std::to_string(i);
            if (item != expected_item) {
                std::cout << "GOT " << item << ", EXPECTED " << expected_item << '\n';
            }
        }
        std::cout << "CHECKED " << size << " BYTES\n";
    }
    for (int _=0;_<4;++_) {
        std::cout << "FINISH..." << '\n';
        typedef char char6_t[6];
        GzipLogReader reader(path);
        char6_t item_data;
        size_t size = 0;
        size_t i = 100000;
        while (reader.next<char6_t>(item_data) == sizeof(char6_t)) {
            size += sizeof(char6_t);
            const std::string item((const char*) &item_data, sizeof(item_data));
            const std::string expected_item = std::to_string(i);
            if (item != expected_item) {
                std::cout << "GOT " << item << ", EXPECTED " << expected_item << '\n';
            }
            ++i;
        }
        std::cout << "FINISHED, READ " << size << " BYTES\n";
    }
    return 0;
}
