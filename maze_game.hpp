//
// Created by leking on 22-10-1.
//

#ifndef LEKENGINE_MAZE_GAME_HPP
#define LEKENGINE_MAZE_GAME_HPP


#include <vector>
#include "lek_game_object.hpp"

namespace leking {
    struct MazePos {
        int x{};
        int y{};
    };
    struct MazeKeyMapping {
        int resetMaze = GLFW_KEY_R;
        int solveMaze = GLFW_KEY_L;
    };
    struct MazeRouteStack {
        void Push(MazePos pos);
        MazePos Pop();
        MazePos Peek();
        int Size();
    private:
        std::vector<MazePos> data{};
    };

    class MazeGameManager {
    public:

        MazeGameManager(LekDevice& device, std::vector<LekGameObject>& gameObjects, int width, int height);

        void CreateMaze();

        void RefreshMaze();

        bool SolveMaze();

        void Update(GLFWwindow *window);
    private:

        void GenerateMaze();

        void AddWall(int x, int y);

        void AddRoad(int x, int y);

        LekDevice& device;

        std::vector<LekGameObject>& gameObjects;

        shared_ptr<LekModel> wallModel;
        shared_ptr<LekModel> roadModel;

        MazePos currentPos;

        MazeRouteStack mazeRouteStack{};

        bool canSolveMaze{false};

        bool onRefresh{false};
        char* data;
        const int width;
        const int height;

        MazeKeyMapping keys{};

        bool isContinue;
    };

} // leking

#endif //LEKENGINE_MAZE_GAME_HPP
