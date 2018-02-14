#ifndef CTRADING__MODELS__TIMESTAMP__HPP
#define CTRADING__MODELS__TIMESTAMP__HPP


#include <string>
#include <cmath>

#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>


#pragma pack(push, 1)

class Timestamp {
public:

    inline Timestamp(struct tm& t) :
        _value(mktime(&t)) {}
    inline Timestamp(int32_t year, uint8_t month=1, uint8_t day=1, uint8_t hours=0, uint8_t minutes=0, double seconds=0.) {
        struct tm t;
        t.tm_sec = seconds;
        t.tm_min = minutes;
        t.tm_hour = hours;
        t.tm_mday = day;
        t.tm_mon = month - 1;
        t.tm_year = year - 1900;
        t.tm_isdst = 0;
        _value = seconds + mktime(&t);
    }
    inline Timestamp(const struct timeval& tv)
        : _value(tv.tv_sec + tv.tv_usec * 1e-6) {}
    inline Timestamp() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        time_t original = tv.tv_sec;
        double microseconds = tv.tv_usec * 1e-6;
        struct tm* converted = gmtime(&original);
        _value = mktime(converted) + microseconds;
    }
    inline Timestamp(const double value)
        : _value(value) {}
    inline Timestamp(const Timestamp& source)
        : _value(source._value) {}

    inline double& operator * () {
        return _value;
    }
    inline operator const double& () const {
        return _value;
    }
    inline operator double& () {
        return _value;
    }

    inline const bool operator < (const Timestamp& other) const {
        return _value < other._value;
    }
    inline const bool operator <= (const Timestamp& other) const {
        return _value <= other._value;
    }
    inline const bool operator > (const Timestamp& other) const {
        return _value > other._value;
    }
    inline const bool operator >= (const Timestamp& other) const {
        return _value >= other._value;
    }
    inline const bool operator != (const Timestamp& other) const {
        return _value != other._value;
    }
    inline const bool operator == (const Timestamp& other) const {
        return _value == other._value;
    }

    template <typename Numeric, typename std::enable_if<std::is_convertible<Numeric, double>::value>::type* = nullptr>
    inline Timestamp operator + (const Numeric& other) {
        return Timestamp(_value + other);
    }
    template <typename Numeric, typename std::enable_if<std::is_convertible<Numeric, double>::value>::type* = nullptr>
    inline Timestamp operator - (const Numeric& other) {
        return Timestamp(_value - other);
    }
    template <typename Numeric, typename std::enable_if<std::is_convertible<Numeric, double>::value>::type* = nullptr>
    inline void operator += (const Numeric& other) {
        _value += other;
    }
    template <typename Numeric, typename std::enable_if<std::is_convertible<Numeric, double>::value>::type* = nullptr>
    inline void operator -= (const Numeric& other) {
        _value -= other;
    }

    inline operator std::string () const {
        if (std::isnan(_value)) {
            return "????-??-??T??:??:??";
        } else {
            const time_t t = _value;
            struct tm lt;
            localtime_r(&t, &lt);
            static const char* format = "%Y-%m-%dT%H:%M:%S";
            char buffer[32];
            size_t size = strftime(buffer, sizeof(buffer), format, &lt);
            return std::string(buffer, size);
        }
    }

private:

    double _value;

};


struct TimestampSpan {
    inline TimestampSpan(const Timestamp& _from=NAN, const Timestamp _to=NAN) : from(_from), to(_to) {}
    void operator += (const double& value) {
        from += value;
        to += value;
    }
    void operator -= (const double& value) {
        from -= value;
        to -= value;
    }

    Timestamp from;
    Timestamp to;
};


#pragma pack(pop)


#include <ostream>
#include <time.h>
#include <string.h>

inline std::ostream& operator << (std::ostream& os, const Timestamp& timestamp) {
    return (os << std::string(timestamp));
}
inline std::ostream& operator << (std::ostream& os, const TimestampSpan& timestamp_span) {
    return (os
        << "<TimestampSpan"
        << " from=" << timestamp_span.from
        << " to=" << timestamp_span.to
        << ">"
    );
}


#endif // CTRADING__MODELS__TIMESTAMP__HPP
