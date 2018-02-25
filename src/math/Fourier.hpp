#ifndef CPPTRAING__MATH__FOURIER_HPP
#define CPPTRAING__MATH__FOURIER_HPP


#include "range/Range.hpp"
#include "math/fast_trigo.hpp"

#include <string.h>

#include <functional>
#include <vector>
#include <ostream>


struct FourierCoefficient {

    inline FourierCoefficient(const double _period) :
        period(_period) {}

    template <typename T>
    void compute(Range<T>& range, std::function<std::pair<double,double>(T)>& translator) {
        double count = 0;
        double omega = 2. * M_PI / period;
        a = b = 0;
        for (const T& item : range) {
            const auto point = translator(item);
            double x = omega * point.first;
            a += point.second * fast_cos(x);
            b += point.second * fast_sin(x);
            ++count;
        }
        a /= count;
        b /= count;
        modulus = sqrt((a*a + b*b) / period);
    }

    double period;
    double a;
    double b;
    double modulus;
    double norm;

};

std::ostream& operator << (std::ostream& os, const FourierCoefficient& fourier_coefficient) {
    return (os
        << "<FourierCoefficient"
        << " period=" << fourier_coefficient.period
        << " a=" << fourier_coefficient.a
        << " b=" << fourier_coefficient.b
        << " modulus=" << fourier_coefficient.modulus
        << " norm=" << fourier_coefficient.norm
        << ">"
    );
}


template <typename T>
inline T identity(T item) {
    return item;
}


struct FourierAnalysis {
public:

    inline FourierAnalysis(const double _period_min, const double _period_max, const double _period_step) :
        period_min(std::min(_period_min, _period_max)),
        period_max(std::max(_period_min, _period_max)),
        period_step(_period_step) {}

    template <typename T>
    inline FourierAnalysis& compute(Range<T>& range, std::function<std::pair<double,double>(T)> translator) {
        _coefficients.clear();
        double sum;
        for (double period=period_min; period<=period_max; period+=period_step) {
            FourierCoefficient coefficient(period);
            coefficient.compute(range, translator);
            _coefficients.push_back(coefficient);
            sum += coefficient.modulus * coefficient.modulus;
        }
        double sqrt_sum = sqrt(sum);
        for (FourierCoefficient& coefficient : _coefficients) {
            coefficient.norm = coefficient.modulus / sqrt_sum;
        }
        return *this;
    }

    inline FourierAnalysis& compute(Range<std::pair<double,double>>& range) {
        return compute<std::pair<double,double>>(range, identity<std::pair<double,double>>);
    }

    inline const std::vector<FourierCoefficient>& get_coefficients() const {
        return _coefficients;
    }
    inline const FourierCoefficient get_best_coefficient() const {
        FourierCoefficient best_coefficient = {0};
        for (const FourierCoefficient& coefficient : _coefficients) {
            if (coefficient.modulus > best_coefficient.modulus) {
                best_coefficient = coefficient;
            }
        }
        return best_coefficient;
    }

private:

    const double period_min;
    const double period_max;
    const double period_step;
    std::vector<FourierCoefficient> _coefficients;

};


#endif // CPPTRAING__MATH__FOURIER_HPP
