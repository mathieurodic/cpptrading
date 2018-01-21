#ifndef CTRADING__MODELS__TRADEAVERAGE__HPP
#define CTRADING__MODELS__TRADEAVERAGE__HPP


#include <stdint.h>


#pragma pack(push, 1)


struct TradeAveragePart {
    double price;
    double volume;
    inline TradeAveragePart() : price(NAN), volume(0.0) {}
    inline void operator += (const Trade& trade) {
        if (std::isnan(price)) {
            price = trade.price;
        } else {
            price = (price * volume + trade.price * trade.volume) / (volume + trade.volume);
        }
        volume += trade.volume;
    }
    inline void operator -= (const Trade& trade) {
        if (std::isnan(price)) {
            price = trade.price;
        } else {
            price = (price * volume - trade.price * trade.volume) / (volume - trade.volume);
        }
        volume -= trade.volume;
    }
    inline const double compute_spread_with(const TradeAveragePart& other) const {
        return (price * volume + other.price * other.volume) / (volume + other.volume);
    }
};


template <typename timestamp_t>
struct timestamp_range_t {
    template <typename other_timestamp_t> inline const bool operator < (timestamp_range_t<other_timestamp_t>& other) { return start < other.start || (start == other.start && end < other.end); }
    template <typename other_timestamp_t> inline const bool operator > (timestamp_range_t<other_timestamp_t>& other) { return start > other.start || (start == other.start && end > other.end); }
    timestamp_t start;
    timestamp_t end;
};

#include <ostream>

template <typename timestamp_t>
inline std::ostream& operator << (std::ostream& os, const timestamp_range_t<timestamp_t>& timestamp_range) {
    return os << '(' << timestamp_range.start << ',' << timestamp_range.end << ')';
}


struct TradeAverage {
    inline const bool operator==(const TradeAverage& other) { return memcmp(this, &other, sizeof(*this)) == 0; }
    inline TradeAverage(const double timestamp_start=NAN, const double timestamp_end=NAN) : timestamp_range({.start=timestamp_start, .end=timestamp_end}), spread(NAN) {}
    timestamp_range_t<double> timestamp_range;
    TradeAveragePart buys;
    TradeAveragePart sells;
    double spread;
    inline void operator += (const Trade& trade) {
        switch (trade.type) {
            case BUY:
                buys += trade;
                break;
            case SELL:
                sells += trade;
                break;
        }
        spread = buys.compute_spread_with(sells);
    }
    inline void operator -= (const Trade& trade) {
        switch (trade.type) {
            case BUY:
                buys -= trade;
                break;
            case SELL:
                sells -= trade;
                break;
        }
        spread = buys.compute_spread_with(sells);
    }
};


#pragma pack(pop)


#endif // CTRADING__MODELS__TRADEAVERAGE__HPP
