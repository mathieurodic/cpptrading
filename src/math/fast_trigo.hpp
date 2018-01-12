#ifndef CTRADING__MATH__FAST_TRIGO__HPP
#define CTRADING__MATH__FAST_TRIGO__HPP


#include <math.h>


// configuration

#ifndef fast_trigo_type
#define fast_trigo_type double
#endif
#ifndef fast_trigo_resolution_bits
#define fast_trigo_resolution_bits (16)
#endif
const int fast_trigo_resolution = 1 << (fast_trigo_resolution_bits);

// cache initialization

fast_trigo_type fast_cos_cache[fast_trigo_resolution];
fast_trigo_type fast_sin_cache[fast_trigo_resolution];

const bool fast_trigo_initializer() {
    for (int i=0; i<fast_trigo_resolution; ++i) {
        const fast_trigo_type x = 2. * M_PI * ((fast_trigo_type) i - .5) / (fast_trigo_type) fast_trigo_resolution;
        fast_cos_cache[i] = cos(x);
        fast_sin_cache[i] = sin(x);
    }
    return true;
}
const bool fast_trigo_is_initialized = fast_trigo_initializer();

// values computation

const fast_trigo_type fast_trigo_coefficient = .5 * (double) fast_trigo_resolution / M_PI;
const int fast_trigo_mask = fast_trigo_resolution - 1;
#define fast_cos(x) fast_cos_cache[((int) (fast_trigo_coefficient * x)) & fast_trigo_mask]
#define fast_sin(x) fast_sin_cache[((int) (fast_trigo_coefficient * x)) & fast_trigo_mask]


#endif // CTRADING__MATH__FAST_TRIGO__HPP
