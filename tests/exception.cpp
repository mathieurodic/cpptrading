#include "exceptions/Exception.hpp"

#include <iostream>


int main(int argc, char const *argv[]) {
    try {
        throw Exception("Ceci n'est pas un test", 15);
    } catch (const Exception& exception) {
        std::cout << exception.what() << '\n';
    }
    return 0;
}
