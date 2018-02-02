#ifndef CTRADING__BROKERS__BROKER__HPP
#define CTRADING__BROKERS__BROKER__HPP


#include <unistd.h>
#include <sys/time.h>


class Broker {
public:

    inline Broker(History& history, Bot& bot) :
        _history(history),
        _bot(bot),
        _is_running(false) {}

    virtual void execute(Decision& decision) {
        std::cout << decision << '\n';
    }

    static inline double get_timestamp(const struct timeval& tv) {
        return tv.tv_sec + tv.tv_usec * 1e-6;
    }
    static inline double get_timestamp() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return get_timestamp(tv);
    }

    inline void start(const double interval) {
        _is_running = true;
        double previous_timestamp = get_timestamp() - interval;
        while (_is_running) {
            double timestamp;
            do {
                timestamp = get_timestamp();
                usleep(1000);
            } while (_is_running && timestamp - previous_timestamp < interval);
            Decision decision = _bot.decide(_history, timestamp);
            previous_timestamp = timestamp;
            execute(decision);
        }
    }

private:
    bool _is_running;
    History& _history;
    Bot& _bot;
};


#endif // CTRADING__BROKERS__BROKER__HPP
