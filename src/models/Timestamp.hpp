#ifndef CTRADING__MODELS__TIMESTAMP__HPP
#define CTRADING__MODELS__TIMESTAMP__HPP


#pragma pack(push, 1)

template <typename base_type>
class Timestamp {
public:

    inline Timestamp(const base_type value=0.0) : _value(value) {}
    inline operator const base_type& () const {
        return _value;
    }
    inline operator base_type& () {
        return _value;
    }

private:

    base_type _value;
};

#pragma pack(pop)


#include <ostream>
#include <time.h>
#include <cmath>
#include <string.h>

template <typename base_type>
inline std::ostream& operator << (std::ostream& os, const Timestamp<base_type>& timestamp) {
    const double t = timestamp;
    if (std::isnan(t)) {
        return (os << "????-??-??T??:??:??");
    } else {
        const time_t t = timestamp;
        struct tm lt;
        localtime_r(&t, &lt);
        static const char* format = "%Y-%m-%dT%H:%M:%S";
        char buffer[32];
        memset(buffer, 0, sizeof(buffer));
        strftime(buffer, sizeof(buffer), format, &lt);
        return (os << buffer);
    }
}


#endif // CTRADING__MODELS__TIMESTAMP__HPP
