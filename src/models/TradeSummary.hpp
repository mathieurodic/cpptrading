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
        average_price(NAN) {}
    TimestampSpan timestamp_span;
    TradeSummaryPart buys;
    TradeSummaryPart sells;
    double spread;
    double average_price;
    inline void operator += (const Trade& trade) {
        if (std::isnan(*timestamp_span.from) || trade.timestamp < timestamp_span.from) {
            timestamp_span.from = trade.timestamp;
        }
        if (std::isnan(*timestamp_span.to) || trade.timestamp > timestamp_span.to) {
            timestamp_span.to = trade.timestamp;
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
        average_price = (buys.price + sells.price) / (buys.volume + sells.volume);
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
        << " from=" << trade_summary.timestamp_span.from
        << " to=" << trade_summary.timestamp_span.to
        << " buys=" << trade_summary.buys
        << " sells=" << trade_summary.sells
        << " spread=" << trade_summary.spread
        << ">"
    );
}


#endif // CTRADING__MODELS__TRADESUMMARY__HPP
