#ifndef CTRADING__MODELS__TRADESUMMARY__HPP
#define CTRADING__MODELS__TRADESUMMARY__HPP


#include <stdint.h>
#include <ostream>


#pragma pack(push, 1)


struct TradeSummaryPart {
    size_t count;
    double price;
    double volume;
    double average_price;
    inline TradeSummaryPart() : price(0.0), volume(0.0), count(0), average_price(NAN) {}
    inline void operator += (const Trade& trade) {
        ++count;
        volume += trade.volume;
        price += trade.volume * trade.price;
        average_price = price / volume;
    }
    inline const double compute_spread_with(const TradeSummaryPart& other) const {
        return std::abs(price - other.price) / (volume + other.volume);
    }
};


struct TradeSummary {
    inline const bool operator==(const TradeSummary& other) { return memcmp(this, &other, sizeof(*this)) == 0; }
    inline TradeSummary() :
        spread(NAN),
        timestamp_begin(NAN),
        timestamp_end(NAN) {}
    Timestamp timestamp_begin;
    Timestamp timestamp_end;
    TradeSummaryPart buys;
    TradeSummaryPart sells;
    double spread;
    inline void operator += (const Trade& trade) {
        if (std::isnan(*timestamp_begin) || trade.timestamp < timestamp_begin) {
            timestamp_begin = trade.timestamp;
        }
        if (std::isnan(*timestamp_end) || trade.timestamp > timestamp_end) {
            timestamp_end = trade.timestamp;
        }
        switch (trade.type) {
            case BUY:
                buys += trade;
                break;
            case SELL:
                sells += trade;
                break;
            case UNDEFINED:
                buys += trade / 2.;
                sells += trade / 2.;
                break;
        }
        spread = buys.compute_spread_with(sells);
    }
};

#pragma pack(pop)


inline std::ostream& operator << (std::ostream& os, const TradeSummaryPart trade_summary_part) {
    return (os
        << "("
        << "count=" << trade_summary_part.count
        << " volume=" << trade_summary_part.volume
        << " average_price=" << trade_summary_part.average_price
        << ")"
    );
}
inline std::ostream& operator << (std::ostream& os, const TradeSummary trade_summary) {
    return (os
        << "<TradeSummary"
        << " timestamp_begin=" << trade_summary.timestamp_begin
        << " timestamp_end=" << trade_summary.timestamp_end
        << " buys=" << trade_summary.buys
        << " sells=" << trade_summary.sells
        << " spread=" << trade_summary.spread
        << ">"
    );
}


#endif // CTRADING__MODELS__TRADESUMMARY__HPP
