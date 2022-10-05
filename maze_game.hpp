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
        int autoSolveMaze = GLFW_KEY_O;
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

        int StepMaze();

        void Update(GLFWwindow *window, float dt);
    private:

        void GenerateMaze();

        void AddWall(int x, int y);

        void AddWalkThrough(int x, int y);

        void AddRoad(int x, int y);

        LekDevice& device;

        std::vector<LekGameObject>& gameObjects;

        shared_ptr<LekModel> wallModel;
        shared_ptr<LekModel> roadModel;

        MazePos currentPos;

        MazeRouteStack mazeRouteStack{};

        bool canSolveMaze{false};

        bool startSolveMaze{false};

        bool solvedSuccessfully{false};

        bool autoMod{false};

        bool onRefresh{false};
        char* data;
        const int width;
        const int height;

        MazeKeyMapping keys{};

        float count{0};

        void PopWall();

        void SolveMaze();
    };

} // leking

#endif //LEKENGINE_MAZE_GAME_HPP
