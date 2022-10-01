
#include "lek_app.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
using namespace std;
int main() {
    leking::LekApp app{};

    try {
        app.run();
    } catch (const exception &e) {
        cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}