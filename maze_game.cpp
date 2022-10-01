//
// Created by leking on 22-10-1.
//

#include <cstring>
#include <iostream>
#include "maze_game.hpp"

namespace leking {
    MazeGameManager::MazeGameManager(int width, int height) : width(width), height(height){
        if(width < 3 || height < 3){
            throw runtime_error("迷宫长宽不能小于3");
        }
        data = (char*)malloc(sizeof(char)*width*height);
        memset(data,'o',sizeof(char)*width*height);
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                if(i == 0 || i == width - 1 || j == 0 || j == height - 1) {
                    data[height*i + j] = 'x';
                    continue;
                }
                if(rand()%2 == 0) data[height*i + j] = 'o';
                else data[height*i + j] = 'x';
            }
            std::cout << endl;
        }
        data[height*0+1] = 'o';
        data[height*(width-1)+(height-2)] = 'o';
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                std::cout << data[height*i + j];
            }
            std::cout << endl;
        }
    }

    std::unique_ptr<LekModel> createWall(LekDevice& device, glm::vec3 offset){
        glm::vec3 color = {0.8627f,0.5862f,0.9450};
        LekModel::Builder modelBuilder{};
        modelBuilder.vertices = {
                // left face (white)
                {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
                {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
                {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
                {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

                // right face (yellow)
                {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
                {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
                {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
                {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

                // top face (orange, remember y axis points down)
                {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
                {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
                {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

                // bottom face (red)
                {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
                {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
                {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

                // nose face (blue)
                {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
                {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
                {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
                {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

                // tail face (green)
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
                {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
        return std::make_unique<LekModel>(device, modelBuilder);
    }

    void MazeGameManager::CreateMaze(LekDevice& device ,std::vector<LekGameObject>& gameObjects) {

        for(int i = 0;i< gameObjects.size();i++) {
            if(gameObjects[i].name.substr(0,4) == "wall") {
                gameObjects.erase(gameObjects.begin() + i);
            }
        }

        std::shared_ptr<LekModel> lekModel = createWall(device, {0.0f, 0.0f, 0.0f});
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                if(data[width*i+j] == 'x')
                {
                    auto wall = LekGameObject::createGameObject();
                    wall.model = lekModel;
                    wall.name = "wall("+ to_string(i) + to_string(j)+")";
                    wall.transform.translation = {i*0.5f, 0.0f, j*0.5f};
                    wall.transform.scale = {0.5f, 0.5f, 0.5f};
                    gameObjects.push_back(std::move(wall));
                }
            }
        }
    }

    void MazeGameManager::Update(LekDevice& device, GLFWwindow *window, vector<LekGameObject> &gameObjects) {
        if(glfwGetKey(window, keys.resetMaze) == GLFW_PRESS && !OnRefresh) {
            OnRefresh = true;
            RefreshMaze();
            CreateMaze(device, gameObjects);
        }
        else if(glfwGetKey(window, keys.resetMaze) == GLFW_RELEASE && OnRefresh) {
            OnRefresh = false;
        }
    }

    void MazeGameManager::RefreshMaze() {
        free(data);
        data = (char*)malloc(sizeof(char)*width*height);
        memset(data,'o',sizeof(char)*width*height);
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                if(i == 0 || i == width - 1 || j == 0 || j == height - 1) {
                    data[height*i + j] = 'x';
                    continue;
                }
                if(rand()%2 == 0) data[height*i + j] = 'o';
                else data[height*i + j] = 'x';
            }
            std::cout << endl;
        }
        data[height*0+1] = 'o';
        data[height*(width-1)+(height-2)] = 'o';
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                std::cout << data[height*i + j];
            }
            std::cout << endl;
        }
    }
} // leking