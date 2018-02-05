#ifndef CPPTRAING__SOURCES__BITSTAMPSOURCE_HPP
#define CPPTRAING__SOURCES__BITSTAMPSOURCE_HPP


#include <fstream>
#include <sstream>
#include <string>

#include "./Source.hpp"
#include "exceptions/Exception.hpp"


class CSVSource : public Source {
public:

    inline CSVSource(const std::string& currency_pair, const std::string& path) :
        Source(currency_pair),
        _path(path) {}

    inline void parse() {
        FILE* file = fopen(_path.c_str(), "r");
        if (file == NULL) {
            throw FileException("CSVSource could not open file for reading", _path, strerror(errno));
        }
        while (true) {
            Trade trade = {0};
            const int result = fscanf(file, "%lf,%lf,%lf\n", &trade.timestamp, &trade.price, &trade.volume);
            if (result == EOF || result != 3) {
                break;
            }
            feed(trade);
        }
    }

private:

    const std::string _path;

};


#endif // CPPTRAING__SOURCES__BITSTAMPSOURCE_HPP
