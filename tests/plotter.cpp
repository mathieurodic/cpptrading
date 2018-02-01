#include "math/Plotter.hpp"


class MyPlotter : public Plotter {
    virtual void on_key_press(const int key) {
        switch (key) {
            case 81:
            case 113:
                _is_looping = false;
                break;
            case 65:
                axes.y.min += .5;
                axes.y.max = NAN;
                break;
            case 66:
                axes.y.min -= .5;
                axes.y.max = NAN;
                break;
            case 67:
                axes.x.min += .5;
                axes.x.max = NAN;
                break;
            case 68:
                axes.x.min -= .5;
                axes.x.max = NAN;
                break;
        }
        show();
        mvprintw(0, 0, "%-6d", key);
        refresh();
    }
};


int main (int argc, char **argv)
{
    MyPlotter plotter;
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
