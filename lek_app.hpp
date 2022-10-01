//
// Created by leking on 22-9-28.
//

#ifndef LEKENGINE_LEK_APP_HPP
#define LEKENGINE_LEK_APP_HPP

#include "lek_window.hpp"
#include "lek_game_object.hpp"
#include "lek_device.hpp"
#include "lek_renderer.hpp"

#include <memory>
#include <vector>

using namespace std;
namespace leking {
    class LekApp {
    public:
        static constexpr int WIDTH = 1600;
        static constexpr int HEIGHT = 1600;

        LekApp();
        ~LekApp();

        LekApp(const LekApp &) = delete;
        LekApp& operator=(const LekApp&) = delete;

        void run();

    private:
        void loadGameObjects();

        LekWindow lekWindow{WIDTH, HEIGHT, "LekEngine"};
        LekDevice lekDevice{lekWindow};
        LekRenderer lekRenderer{lekDevice,lekWindow};

        std::vector<LekGameObject> gameObjects;
    };
}

#endif //LEKENGINE_LEK_APP_HPP
