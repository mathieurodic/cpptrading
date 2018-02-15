#include <iostream>

#include "math/Plotter.hpp"


class MyPlotter : public Plotter {
    virtual void on_key_press(const int key) {
        Plotter::on_key_press(key);
    }
};


int main (int argc, char **argv)
{
    MyPlotter plotter;
    //
    plotter.axes.x.type = PlotterAxisParameters::LINEAR;
    plotter.axes.x.min = -8.;
    plotter.axes.x.max = +8.;
    plotter.axes.x.origin = 0.;
    plotter.axes.x.grid = 1.;
    //
    // plotter.axes.y.type = PlotterAxisParameters::LOGARITHMIC;
    // plotter.axes.y.min = 1.;
    // plotter.axes.y.max = 8.;
    // plotter.axes.y.origin = 1.;
    // plotter.axes.y.grid = 2.;
    plotter.axes.y.type = PlotterAxisParameters::LINEAR;
    plotter.axes.y.min = -4.;
    plotter.axes.y.max = +4.;
    plotter.axes.y.origin = 0.;
    plotter.axes.y.grid = 1.;
    //
    const double origin = 2.;
    plotter.plot([origin](double x) {
        return x * x - origin;
    }, GREEN);
    plotter.plot([](double x_min, double x_max) {
        return std::pair<double, double>(x_min, x_max);
    });
    plotter.plot(sqrt, BLUE);
    plotter.plot(exp, YELLOW);
    plotter.plot(log, MAGENTA);
    plotter.plot(sin, RED);
    plotter.plot({{0, 0}, {-1, 1}, {-2, 2}, {1, -1}, {-.5, .5}, {-1.5, 1.5}, {-42, 42}}, CYAN);
    plotter.start();
    return 0;
}
