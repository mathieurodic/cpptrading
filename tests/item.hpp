#ifndef CPPTRADINT__TESTS__ITEM
#define CPPTRADINT__TESTS__ITEM


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


#endif // CPPTRADINT__TESTS__ITEM
