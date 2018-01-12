#define fast_trigo_type double
#define fast_trigo_resolution_bits 16
#include "math/fast_trigo.hpp"


#include <vector>
#include <iostream>


int main(int argc, char const *argv[]) {

    std::vector<double> values;

    const fast_trigo_type dx = 1e-2;
    fast_trigo_type sum = 0.;
    for (fast_trigo_type x=0; x<2e5; x+=dx) {
        // values.push_back(((fast_trigo_type)M_E) * fast_sin(x));
        sum += fast_sin(x);
    }

    fast_trigo_type diff = 0.;
    fast_trigo_type count;
    for (fast_trigo_type x=0; x<2e5; x+=dx) {
        fast_trigo_type dy = fabs(fast_sin(x) - sin(x));
        diff += dy * dy;
        ++count;
    }
    std::cout << (sqrt(diff) / count) << '\n';

    return 0;
}
