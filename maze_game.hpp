//
// Created by leking on 22-10-1.
//

#ifndef LEKENGINE_MAZE_GAME_HPP
#define LEKENGINE_MAZE_GAME_HPP


#include <vector>
#include "lek_game_object.hpp"

namespace leking {
    struct MazeKeyMapping {
        int resetMaze = GLFW_KEY_R;
    };

    class MazeGameManager {
    public:

        MazeGameManager(int width, int height);

        void CreateMaze(LekDevice& device, std::vector<LekGameObject>& gameObjects);

        void RefreshMaze();

        void Update(LekDevice& device, GLFWwindow *window, std::vector<LekGameObject>& gameObjects);
    private:
        bool OnRefresh{false};
        char* data;
        const int width;
        const int height;

        MazeKeyMapping keys{};

        bool isContinue;
    };

} // leking

#endif //LEKENGINE_MAZE_GAME_HPP
