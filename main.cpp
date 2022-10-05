
#include "lek_app.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
using namespace std;
int main() {

    int mw = 10, mh = 10;

    std::cout<<"请输入迷宫的宽度"<<endl;
    std::cin>> mw;
    std::cout<<"请输入迷宫的宽度"<<endl;
    std::cin>> mh;

    leking::LekApp app{};

    try {
        app.run(mw, mh);
    } catch (const exception &e) {
        cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}