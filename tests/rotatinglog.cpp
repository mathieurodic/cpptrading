#include "db/RotatingLog.hpp"

#include <iostream>
#include <ostream>

#include <stdlib.h>
#include <unistd.h>


#pragma pack(push, 1)

struct item_t {

    inline item_t(const double& number=0.) {
        integer_number = number;
        floating_number = number;
        memset(text, 0, sizeof(text));
        snprintf(text, sizeof(text), "%lf<", number);
    }
    inline const bool operator == (const item_t& other) const {
        return integer_number == other.integer_number
            && floating_number == other.floating_number
            && !memcmp(text, other.text, sizeof(text));
    }
    inline const bool operator != (const item_t& other) const {
        return integer_number != other.integer_number
            || floating_number != other.floating_number
            || memcmp(text, other.text, sizeof(text));
    }

    int integer_number;
    double floating_number;
    char text[32];
};

#pragma pack(pop)


inline std::ostream& operator << (std::ostream& os, const item_t& item) {
    return (os
        << "<Item "
        << item.floating_number
        << '>'
    );
}


int main(int argc, char const *argv[]) {


    const std::string basepath = "/tmp/cpptrading-test-rotatinglog";
    const int seed = 123;
    const int count = 100000;
    const double milliseconds = .1;

    if (execl("rm", (basepath + '*').c_str()) == -1) {
        std::cerr << "rm " << basepath << "* : " << strerror(errno) << '\n';
    }

    std::cout << "\n\nTEST WRITING\n\n";
    {
        RotatingLogWriter logger(basepath, 1);
        srand(seed);
        for (size_t i=0; i<count; ++i) {
            item_t item(rand());
            // std::cout << item << '\n';
            logger.append(item);
            if (milliseconds) {
                usleep(milliseconds * 1000);
            }
        }
    }

    std::cout << "\n\nTEST READING\n\n";
    {
        RotatingLogReader logger(basepath);
        srand(seed);
        item_t item;
        for (size_t i=0; i<count; ++i) {
            item_t expected_item(rand());
            if (!logger.next(item)) {
                std::cerr << "ERROR: COULD NOT FIND ITEM" << '\n';
            } else if (item != expected_item) {
                std::cerr << "ERROR: GOT " << item << ", EXPECTED " << expected_item << '\n';
            }
        }
        if (logger.next(item)) {
            std::cerr << "ERROR: TOO MANY ITEMS" << '\n';
        }
    }

    return 0;
}
