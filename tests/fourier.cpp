#include <vector>
#include <iostream>

#include "math/Fourier.hpp"
#include "range/ForwardRange.hpp"



const int n = 1000;
const double x_min = 0.;
const double x_max = 100.;

const double period = 100;
const double period_min = 50;
const double period_max = 150;
const double period_step = 1;



struct Point {
    double x;
    double y;
};


int main(int argc, char const *argv[]) {

    // create values
    std::vector<Point> values;
    for (int i=0; i<n; ++i) {
        const double x = x_min + (x_max - x_min) * ((double)rand() / (double)RAND_MAX);
        const double y = sin(2 * M_PI / period * x);
        values.push_back({
            .x = x,
            .y = y
        });
    }

    // compute & display fourier
    FourierAnalysis analysis(period_min, period_max, period_step);
    Range<Point> range = ForwardRangeFactory(values);
    analysis.compute<Point>(range, [](Point point) {
        return std::make_pair(point.x, point.y);
    });
    for (const FourierCoefficient& coefficient : analysis.get_coefficients()) {
        std::cout << coefficient << '\n';
    }

    // show best coefficient
    std::cout << '\n';
    std::cout << analysis.get_best_coefficient() << '\n';

    return 0;
}
