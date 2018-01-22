#include "bots/Bot.hpp"


int main(int argc, char const *argv[]) {
    std::cout << "Advice objects are " << sizeof(Advice) << " bytes long.\n";
    #define SHOW_ADVICE_PROPERTY(PROPERTY) std::cout << "Advice()." #PROPERTY " = " << Advice().PROPERTY << '\n'
    SHOW_ADVICE_PROPERTY(type);
    SHOW_ADVICE_PROPERTY(volume);
    SHOW_ADVICE_PROPERTY(minimum_price);
    SHOW_ADVICE_PROPERTY(maximum_price);
    SHOW_ADVICE_PROPERTY(timestamp_advice);
    SHOW_ADVICE_PROPERTY(timestamp_execution);
    SHOW_ADVICE_PROPERTY(confidence);
    return 0;
}
