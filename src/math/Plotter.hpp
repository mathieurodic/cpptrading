#ifndef CPPTRAING__MATH__PLOTTER_HPP
#define CPPTRAING__MATH__PLOTTER_HPP


#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// #include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <wchar.h>

#include <vector>
#include <string>


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
    PlotterCurve(double (*f1)(double), PlotterColor color) :
        type(FUNCTION_1),
        f1(f1),
        color(color) {}
    PlotterCurve(double (*f2)(double, double), PlotterColor color) :
        type(FUNCTION_2),
        f2(f2),
        color(color) {}
    PlotterCurve(std::pair<double, double> (*f3)(double, double), PlotterColor color) :
        type(FUNCTION_3),
        f3(f3),
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
    union {
        double (*f1)(double);
        double (*f2)(double, double);
        std::pair<double, double> (*f3)(double, double);
    };
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
    double min;
    double max;
    double step;
    double origin;
    double grid;
};


struct PlotterAxesParameters {
    PlotterAxisParameters x;
    PlotterAxisParameters y;
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
        if (isnan(axis.min)) {
            axis.min = axis.max - (size - 1) * axis.step;
        }
        if (isnan(axis.max)) {
            axis.max = axis.min + (size - 1) * axis.step;
        }
        if (isnan(axis.step)) {
            axis.step = (axis.max - axis.min) / (double) (size - 1);
        }
    }

    inline const int16_t x_to_i(const double x) const {
        return round((x - _axes.x.min + .5 * _axes.x.step) / _axes.x.step);
    }
    inline const int16_t y_to_j(const double y) const {
        return round((_axes.y.max - y - .5 * _axes.x.step) / _axes.y.step);
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

    inline void plot_grid_vertical() {
        double x_min = _axes.x.origin;
        while (x_min > _axes.x.min) {
            x_min -= _axes.x.grid;
        }
        // normal grid
        for (double x=x_min; x<=_axes.x.max; x+=_axes.x.grid) {
            if (x > _axes.x.min) {
                int16_t i = x_to_i(x);
                for (int16_t j=0; j<_plot_height; ++j) {
                    _plot[j][i] |= 8;
                }
            }
        }
        // origin
        if (_axes.x.origin >= _axes.x.min && _axes.x.origin <= _axes.x.max) {
            int16_t i = x_to_i(_axes.x.origin);
            for (int16_t j=0; j<_plot_height; ++j) {
                _plot[j][i] |= 32;
            }
        }
    }
    inline void plot_grid_horizontal() {
        double y_min = _axes.y.origin;
        while (y_min > _axes.y.min) {
            y_min -= _axes.y.grid;
        }
        // normal grid
        for (double y=y_min; y<=_axes.y.max; y+=_axes.y.grid) {
            if (y > _axes.y.min) {
                int16_t j = y_to_j(y);
                j -= j % 2;
                for (int16_t i=0; i<_plot_width; ++i) {
                    _plot[j][i] |= 16;
                    _plot[j+1][i] |= 16;
                }
            }
        }
        // origin
        if (_axes.y.origin >= _axes.y.min && _axes.y.origin <= _axes.y.max) {
            int16_t j = y_to_j(_axes.y.origin);
            j -= j % 2;
            for (int16_t i=0; i<_plot_width; ++i) {
                _plot[j][i] |= 32;
                _plot[j+1][i] |= 32;
            }
        }
    }

    inline void plot(double x, double y, PlotterColor color=WHITE) {
        if (isnan(y)) {
            return;
        }
        const int16_t i = x_to_i(x);
        const int16_t j = y_to_j(y);
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

    inline void plot(double (*f)(double), PlotterColor color) {
        for (double x=_axes.x.min; x<_axes.x.max+_axes.x.step; x+=_axes.x.step) {
            plot(x, f(x), color);
        }
    }
    inline void plot(double (*f)(double, double), PlotterColor color) {
        for (double x=_axes.x.min-.5*_axes.x.step; x<_axes.x.max+_axes.x.step; x+=_axes.x.step) {
            plot(x, f(x, x + _axes.x.step), color);
        }
    }
    inline void plot(std::pair<double, double> (*f)(double, double), PlotterColor color) {
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
        // initialize colors
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

    inline void plot(double (*f)(double), PlotterColor color=WHITE) {
        _curves.push_back(PlotterCurve(f, color));
    }
    inline void plot(double (*f)(double, double), PlotterColor color=WHITE) {
        _curves.push_back(PlotterCurve(f, color));
    }
    inline void plot(std::pair<double, double> (*f)(double, double), PlotterColor color=WHITE) {
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

    virtual void on_key_press(const int key) {}
    inline void start() {
        _is_looping = true;
        show();
        while (_is_looping) {
            const int c = getch();
            on_key_press(c);
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