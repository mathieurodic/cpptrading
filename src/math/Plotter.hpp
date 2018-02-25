#ifndef CPPTRAING__MATH__PLOTTER_HPP
#define CPPTRAING__MATH__PLOTTER_HPP


#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <thread>

// #include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <wchar.h>

#include <vector>
#include <string>
#include <functional>

#include "models/Timestamp.hpp"


enum PlotterColor {
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
};


struct PlotterCurve {
    PlotterCurve(std::function<double(double)> f, PlotterColor color) :
        type(FUNCTION_1),
        f1(f),
        color(color) {}
    PlotterCurve(std::function<double(double, double)> f, PlotterColor color) :
        type(FUNCTION_2),
        f2(f),
        color(color) {}
    PlotterCurve(std::function<std::pair<double, double>(double, double)> f, PlotterColor color) :
        type(FUNCTION_3),
        f3(f),
        color(color) {}
    PlotterCurve(std::vector<std::pair<double, double>> values, PlotterColor color) :
        type(VALUES),
        values(values),
        color(color) {}
    enum {
        FUNCTION_1,
        FUNCTION_2,
        FUNCTION_3,
        VALUES,
    } type;

    const std::pair<double, double> compute(std::pair<double, double> x) {
        switch (type) {
            case FUNCTION_1: {
                return {f1(x.first), f1(x.second)};
            }
            case FUNCTION_2: {
                double y = f2(x.first, x.second);
                return {y, y};
            }
            case FUNCTION_3: {
                return f3(x.first, x.second);
            }
            case VALUES: {
                double y_min, y_max;
                for (const auto& value : values) {
                    if (value.first > x.first && value.first <= x.second) {
                        if (std::isnan(y_min) || value.second < y_min) {
                            y_min = value.second;
                        }
                        if (std::isnan(y_max) || value.second < y_max) {
                            y_max = value.second;
                        }
                    }
                }
                return {y_min, y_max};
            }
            default: {
                return {NAN, NAN};
            }
        }
    }

    std::function<double(double)> f1;
    std::function<double(double, double)> f2;
    std::function<std::pair<double, double>(double, double)> f3;
    std::vector<std::pair<double, double>> values;
    PlotterColor color;
};

struct PlotterAxisParameters {
    PlotterAxisParameters() :
        min(NAN),
        max(NAN),
        step(NAN),
        origin(NAN),
        grid(NAN) {}
    enum {TEMPORAL, LINEAR, LOGARITHMIC} type;
    double min;
    double max;
    double step;
    double origin;
    double grid;
};


struct PlotterAxesParameters {
    PlotterAxisParameters x;
    PlotterAxisParameters y;

    inline const double i_to_x(const int16_t value) const {
        switch (x.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR:
                return value * x.step + x.min;
            case PlotterAxisParameters::LOGARITHMIC:
                return NAN;
        }
    }
    inline const int16_t x_to_i(const double value) const {
        switch (x.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR:
                return std::round((value - x.min + .5 * x.step) / x.step);
            case PlotterAxisParameters::LOGARITHMIC:
                if (value < 0) {
                    return -1;
                }
                return std::round(std::log(value / x.min) / x.step);
        }
    }
    inline const int16_t y_to_j(double value) const {
        switch (y.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR:
                return std::round((y.max - value) / y.step);
            case PlotterAxisParameters::LOGARITHMIC:
                if (value <= 0) {
                    return -1;
                }
                return std::round(std::log(y.max / value) / y.step);
        }
    }

};


class PlotterBuffer {
public:

    PlotterBuffer(int16_t plot_width, int16_t plot_height, PlotterAxesParameters& axes) :
        _plot_width(plot_width + 1),
        _plot_height(2 * plot_height + 2),
        _axes(axes)
    {
        // allocate pixels buffer
        _plot = (uint8_t**) malloc(_plot_width * sizeof(uint8_t*));
        _plot_pixels = (uint8_t*) calloc(_plot_width * _plot_height, sizeof(uint8_t));
        for (int16_t j=0; j<_plot_height; ++j) {
            _plot[j] = _plot_pixels + (j * _plot_width);
        }
        // adjust axes
        adjust_axis(_axes.x, _plot_width);
        adjust_axis(_axes.y, _plot_height);
        // draw grids
        plot_grid_vertical();
        plot_grid_horizontal();
    }

    inline void adjust_axis(PlotterAxisParameters& axis, const int16_t size) {
        if (std::isnan(axis.min)) {
            axis.min = axis.max - (size - 1) * axis.step;
        }
        if (std::isnan(axis.max)) {
            axis.max = axis.min + (size - 1) * axis.step;
        }
        if (std::isnan(axis.step)) {
            switch (axis.type) {
                case PlotterAxisParameters::TEMPORAL:
                case PlotterAxisParameters::LINEAR:
                    axis.step = (axis.max - axis.min) / (double) (size - 1);
                    break;
                case PlotterAxisParameters::LOGARITHMIC:
                    axis.step = std::log(axis.max / axis.min) / (double) (size - 1);
                    break;
            }
        }
    }

    inline const bool check_ij(const int16_t i, const int16_t j) const {
        if (i < 0 || i >= _plot_width) {
            return false;
        }
        if (j < 0 || j >= _plot_height) {
            return false;
        }
        return true;
    }

    inline void plot_line_vertical(const double& x, const int value) {
        if (x >= _axes.x.min && x <= _axes.x.max) {
            int16_t i = _axes.x_to_i(x);
            if (i >= 0 && i < _plot_width) {
                for (int16_t j=0; j<_plot_height; ++j) {
                    _plot[j][i] |= value;
                }
            }
        }
    }
    inline void plot_grid_vertical() {
        switch (_axes.x.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR: {
                double x_min = _axes.x.origin;
                while (x_min > _axes.x.min) {
                    x_min -= _axes.x.grid;
                }
                // normal grid
                for (double x=x_min; x<=_axes.x.max; x+=_axes.x.grid) {
                    plot_line_vertical(x, 8);
                }
                // origin
                plot_line_vertical(_axes.x.origin, 32);
            } break;
            case PlotterAxisParameters::LOGARITHMIC: {
                // normal grid
                if (_axes.x.grid > 1.) {
                    double x_min = _axes.x.origin;
                    while (x_min > _axes.x.min) {
                        x_min /= _axes.x.grid;
                    }
                    for (double x=x_min; x<=_axes.x.max; x*=_axes.x.grid) {
                        plot_line_vertical(x, 16);
                    }
                }
                // origin
                plot_line_vertical(_axes.x.origin, 32);
            } break;
        }
    }

    inline void plot_line_horizontal(const double& y, const int value) {
        if (y >= _axes.y.min && y <= _axes.y.max) {
            int16_t j = _axes.y_to_j(y);
            if (j >= 0 && j < _plot_width) {
                j -= j % 2;
                for (int16_t i=0; i<_plot_width; ++i) {
                    _plot[j][i] |= value;
                    _plot[j+1][i] |= value;
                }
            }
        }
    }
    inline void plot_grid_horizontal() {
        switch (_axes.y.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR: {
                double y_min = _axes.y.origin;
                while (y_min > _axes.y.min) {
                    y_min -= _axes.y.grid;
                }
                // normal grid
                for (double y=y_min; y<=_axes.y.max; y+=_axes.y.grid) {
                    plot_line_horizontal(y, 16);
                }
                // origin
                plot_line_horizontal(_axes.y.origin, 32);
            } break;
            case PlotterAxisParameters::LOGARITHMIC: {
                // normal grid
                if (_axes.y.grid > 1.) {
                    double y_min = _axes.y.origin;
                    while (y_min > _axes.y.min) {
                        y_min /= _axes.y.grid;
                    }
                    for (double y=y_min; y<=_axes.y.max; y*=_axes.y.grid) {
                        plot_line_horizontal(y, 16);
                    }
                }
                // origin
                plot_line_horizontal(_axes.y.origin, 32);
            } break;
        }
    }

    inline void plot(double x, double y, PlotterColor color=WHITE) {
        if (std::isnan(y)) {
            return;
        }
        const int16_t i = _axes.x_to_i(x);
        const int16_t j = _axes.y_to_j(y);
        if (!check_ij(i, j)) {
            return;
        }
        _plot[j][i] = color;
    }
    inline void plot(double x, std::pair<double, double> y_range, PlotterColor color=WHITE) {
        for (double y=y_range.first; y<=y_range.second; y+=_axes.y.step) {
            plot(x, y, color);
        }
        plot(x, y_range.second, color);
    }

    inline void plot(std::function<double(double)> f, PlotterColor color) {
        for (double x=_axes.x.min; x<_axes.x.max+_axes.x.step; x+=_axes.x.step) {
            plot(x, f(x), color);
        }
    }
    inline void plot(std::function<double(double, double)> f, PlotterColor color) {
        for (double x=_axes.x.min-.5*_axes.x.step; x<_axes.x.max+_axes.x.step; x+=_axes.x.step) {
            plot(x, f(x, x + _axes.x.step), color);
        }
    }
    inline void plot(std::function<std::pair<double, double>(double, double)> f, PlotterColor color) {
        for (double x=_axes.x.min-.5*_axes.x.step; x<_axes.x.max+_axes.x.step; x+=_axes.x.step) {
            plot(x, f(x, x + _axes.x.step), color);
        }
    }
    inline void plot(const std::vector<std::pair<double, double>> values, PlotterColor color) {
        for (const auto& value : values) {
            plot(value.first, value.second, color);
        }
    }

    inline void plot(const PlotterCurve& curve) {
        switch (curve.type) {
            case PlotterCurve::FUNCTION_1:
                plot(curve.f1, curve.color);
                break;
            case PlotterCurve::FUNCTION_2:
                plot(curve.f2, curve.color);
                break;
            case PlotterCurve::FUNCTION_3:
                plot(curve.f3, curve.color);
                break;
            case PlotterCurve::VALUES:
                plot(curve.values, curve.color);
                break;
        }
    }

    inline wchar_t* get_character(uint8_t pixel_top, uint8_t pixel_bottom) {
        if (!(pixel_top | pixel_bottom)) {
            return L" ";
        } else if (pixel_top < 8 || pixel_bottom < 8) {
            // printf("\e[3%hhu;4%hhum▀", pixel_top & 7, pixel_bottom & 7);
            return L"▀";
        } else {
            switch (pixel_top & 24) {
                case 8:
                    return L"│";
                case 16:
                    return L"─";
                case 24:
                    return L"┼";
                default:
                    return L" ";
            }
        }
    }
    inline void show() {
        for (int16_t j=0; j<_plot_height; j+=2) {
            for (int16_t i=0; i<_plot_width; ++i) {
                const uint8_t pixel_top = _plot[j][i];
                const uint8_t pixel_bottom = _plot[j + 1][i];
                wchar_t* character = get_character(pixel_top, pixel_bottom);
                if (pixel_top < 8 || pixel_bottom < 8) {
                    attron(A_INVIS);
                    // attrset(COLOR_PAIR(2));
                    attrset(COLOR_PAIR((pixel_top & 7) | ((pixel_bottom & 7) << 3)));
                    // attron(COLOR_PAIR((pixel_top & 7) | ((pixel_bottom & 7) << 3)));
                } else if (pixel_top | pixel_bottom) {
                    attrset(COLOR_PAIR(64));
                    if ((pixel_bottom | pixel_top) & 32) {
                        attron(A_BOLD);
                    } else {
                        attron(A_DIM);
                    }
                } else {
                    attrset(COLOR_PAIR(1));
                }
                mvaddwstr(j/2, i, character);
                // attron(COLOR_PAIR(0));
                attroff(A_BOLD);
                attroff(A_DIM);
            }
        }
        switch (_axes.x.type) {
            case PlotterAxisParameters::LINEAR:
            case PlotterAxisParameters::LOGARITHMIC:
                mvprintw(0, 0, " [ %lf , %lf ]  ->  [ %lf , %lf ] ", _axes.x.min, _axes.x.max, _axes.y.min, _axes.y.max);
                break;
            case PlotterAxisParameters::TEMPORAL: {
                const std::string min = Timestamp(_axes.x.min);
                const std::string max = Timestamp(_axes.x.max);
                mvprintw(0, 0, " [ %s , %s ]  ->  [ %lf , %lf ] ", min.c_str(), max.c_str(), _axes.y.min, _axes.y.max);
                }
                break;
        }
        refresh();
    }

private:
    PlotterAxesParameters& _axes;
    int16_t _plot_width, _plot_height;
    uint8_t** _plot;
    uint8_t* _plot_pixels;
};


class Plotter {
public:
    inline Plotter() : _last_window_size({0}), _is_looping(false) {
    	setlocale(LC_ALL, "");
        initscr();
        start_color();
    	clear();
    	noecho();
    	cbreak();
        // initialize palette
        for (int pixel_bottom=0; pixel_bottom<8; ++pixel_bottom) {
            for (int pixel_top=0; pixel_top<8; ++pixel_top) {
                init_pair(pixel_top | (pixel_bottom << 3), pixel_top, pixel_bottom);
            }
        }
        init_pair(64, 7, 0);
    }
    inline ~Plotter() {
        clrtoeol();
    	refresh();
    	endwin();
    }

    inline void clear() {
        _curves.clear();
    }

    inline void plot(std::function<double(double)> f, PlotterColor color=WHITE) {
        _curves.push_back(PlotterCurve(f, color));
    }
    inline void plot(std::function<double(double, double)> f, PlotterColor color=WHITE) {
        _curves.push_back(PlotterCurve(f, color));
    }
    inline void plot(std::function<std::pair<double, double>(double, double)> f, PlotterColor color=WHITE) {
        _curves.push_back(PlotterCurve(f, color));
    }
    inline void plot(std::vector<std::pair<double, double>> values, PlotterColor color=WHITE) {
        _curves.push_back(PlotterCurve(values, color));
    }

    inline void show() {
        // get window size
        struct winsize window_size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size);
        // initialize buffer
        PlotterBuffer buffer(window_size.ws_col, window_size.ws_row - 1, axes);
        if (_last_window_size.ws_col != window_size.ws_col || _last_window_size.ws_row != window_size.ws_row) {
            // buffer.clear();
        }
        _last_window_size = window_size;
        // draw stuff
        for (const PlotterCurve& curve : _curves) {
            buffer.plot(curve);
        }
        // go!
        buffer.show();
    }

    void increase(PlotterAxisParameters& axis) {
        switch (axis.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR: {
                const double delta = (axis.max - axis.min) / 10.;
                axis.min += delta;
                axis.max += delta;
                break;
            }
            case PlotterAxisParameters::LOGARITHMIC: {
                const double factor = pow(axis.max / axis.min, .1);
                axis.min *= factor;
                axis.max *= factor;
                break;
            }
        }
    }
    void decrease(PlotterAxisParameters& axis) {
        switch (axis.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR: {
                const double delta = (axis.max - axis.min) / 10.;
                axis.min -= delta;
                axis.max -= delta;
                break;
            }
            case PlotterAxisParameters::LOGARITHMIC: {
                const double factor = pow(axis.max / axis.min, .1);
                axis.min /= factor;
                axis.max /= factor;
                break;
            }
        }
    }
    void zoomin(PlotterAxisParameters& axis) {
        switch (axis.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR: {
                const double delta = axis.max - axis.min;
                axis.min += delta / 4.;
                axis.max -= delta / 4.;
                axis.step = NAN;
                break;
            }
            case PlotterAxisParameters::LOGARITHMIC: {
                const double ratio = axis.max / axis.min;
                axis.min *= pow(ratio, .25);
                axis.max /= pow(ratio, .25);
                axis.step = NAN;
                break;
            }
        }
    }
    void zoomout(PlotterAxisParameters& axis) {
        switch (axis.type) {
            case PlotterAxisParameters::TEMPORAL:
            case PlotterAxisParameters::LINEAR: {
                const double delta = axis.max - axis.min;
                axis.min -= delta / 2.;
                axis.max += delta / 2.;
                axis.step = NAN;
                break;
            }
            case PlotterAxisParameters::LOGARITHMIC:
                const double ratio = axis.max / axis.min;
                axis.min /= sqrt(ratio);
                axis.max *= sqrt(ratio);
                axis.step = NAN;
                break;
        }
    }

    virtual void on_key_press(const int key) {
        switch (key) {
            case 113:
                _is_looping = false;
                break;
            case KEY_UP:
                increase(axes.y);
                break;
            case KEY_DOWN:
                decrease(axes.y);
                break;
            case KEY_RIGHT:
                increase(axes.x);
                break;
            case KEY_LEFT:
                decrease(axes.x);
                break;
            case 43:
                zoomin(axes.x);
                break;
            case 45:
                zoomout(axes.x);
                break;
            case 42:
                zoomin(axes.y);
                break;
            case 47:
                zoomout(axes.y);
                break;
            case 104:
                if (axes.x.type == PlotterAxisParameters::TEMPORAL) {
                    axes.x.grid = 3600.;
                }
                break;
            case 100:
                if (axes.x.type == PlotterAxisParameters::TEMPORAL) {
                    axes.x.grid = 24. * 3600.;
                }
                break;
            case 119:
                if (axes.x.type == PlotterAxisParameters::TEMPORAL) {
                    axes.x.grid = 7. * 24. * 3600.;
                }
                break;
            case 109:
                if (axes.x.type == PlotterAxisParameters::TEMPORAL) {
                    axes.x.grid = 365.259636 * 24. * 3600. / 12.;
                }
                break;
            case 121:
                if (axes.x.type == PlotterAxisParameters::TEMPORAL) {
                    axes.x.grid = 365.259636 * 24. * 3600.;
                }
                break;
            default:
                return;
        }
        show();
        // static std::vector<int> keys;
        // keys.push_back(key);
        // int i = 0;
        // for (auto it=keys.rbegin(); it!=keys.rend() && i<4; ++it, ++i) {
        //     mvprintw(3+i, 1, "%-3d", *it);
        // }
        refresh();
    }
    virtual void on_mouse_move(const int i, const int j) {
        std::pair<double, double> x = {axes.i_to_x(i), axes.i_to_x(i + 1)};
        attrset(COLOR_PAIR(0));
        mvprintw(3, 1, "x ∈ [%lf ; %lf]", x.first, x.second);
        int n = 0;
        for (auto& curve : _curves) {
            const auto y = curve.compute(x);
            attrset(COLOR_PAIR(curve.color));
            mvprintw(n + 4, 1, "y%d ∈ [%lf ; %lf]", n, y.first, y.second);
            ++n;
        }
    }

    inline void start() {
        _is_looping = true;
        show();
        keypad(stdscr, TRUE);
        mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
        while (_is_looping) {
            const int c = getch();
            MEVENT mouse_event;
            switch (c) {
                case KEY_MOUSE:
                    if (getmouse(&mouse_event) == OK) {
                        on_mouse_move(mouse_event.x, mouse_event.y);
                    }
                    break;
                case ERR:
                    break;
                default:
                    on_key_press(c);
                    break;
            }
            std::this_thread::sleep_for(std::chrono::duration<double>(1e-6));
        }
    }
    inline void stop() {
        _is_looping = false;
    }

    PlotterAxesParameters axes;

protected:
    bool _is_looping;
private:
    std::vector<PlotterCurve> _curves;
    struct winsize _last_window_size;
};


#endif // CPPTRAING__MATH__PLOTTER_HPP
