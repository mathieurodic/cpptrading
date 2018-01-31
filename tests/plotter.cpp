#include "math/Plotter.hpp"


int main (int argc, char **argv)
{
    Plotter plotter;
    //
    plotter.axes.x.min = -2. * M_PI;
    plotter.axes.x.max = +2. * M_PI;
    plotter.axes.x.origin = 0;
    plotter.axes.x.grid = 1;
    //
    plotter.axes.y.min = -2.;
    plotter.axes.y.max = +2.;
    plotter.axes.y.origin = 0;
    plotter.axes.y.grid = 1;
    //
    plotter.plot([](double x) {
        return x * x - 2.;
    }, YELLOW);
    plotter.plot(sqrt, BLUE);
    plotter.plot(exp, CYAN);
    plotter.plot(log, MAGENTA);
    plotter.plot(sin, RED);
    plotter.plot({{0, 0}, {1, 1}, {2, 2}, {-1, -1}, {.5, .5}, {1.5, 1.5}, {-42, 111}}, GREEN);
    plotter.show();
    return 0;
}
