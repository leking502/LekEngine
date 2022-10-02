//
// Created by leking on 22-10-1.
//

#include <cstring>
#include <iostream>
#include "maze_game.hpp"

#define OPENNESS 3

namespace leking {
    MazeGameManager::MazeGameManager(LekDevice& device, std::vector<LekGameObject>& gameObjects, int width, int height) : device(device), gameObjects(gameObjects), width(width), height(height){
        wallModel = LekModel::createModelFromFile(device, "/home/leking/CLionProjects/LekEngine/models/colored_cube.obj");
        roadModel = LekModel::createModelFromFile(device, "/home/leking/CLionProjects/LekEngine/models/cube.obj");
        GenerateMaze();
    }

    void MazeGameManager::GenerateMaze() {
        data = (char*)malloc(sizeof(char)*width*height);
        memset(data,'o',sizeof(char)*width*height);
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                if(i == 0 || i == width - 1 || j == 0 || j == height - 1) {
                    data[height*i + j] = 'x';
                    continue;
                }
                if(rand()%OPENNESS == 0) data[height*i + j] = 'x';
                else data[height*i + j] = 'o';
            }
            //std::cout << endl;
        }
        data[height*0+1] = 'o';
        data[height*(width-1)+(height-2)] = 'o';
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                //std::cout << data[height*i + j];
            }
            //std::cout << endl;
        }
    }

    void MazeGameManager::CreateMaze() {

        for(int i = 0;i< gameObjects.size();i++) {
            if(gameObjects[i].name.substr(0,4) == "wall" || gameObjects[i].name.substr(0,4) == "road") {
                LekGameObject::Destroy(gameObjects[i]);
            }
        }

        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                if(data[width*i+j] == 'x')
                {
                    auto wall = LekGameObject::createGameObject();
                    wall.model = wallModel;
                    wall.name = "wall("+ to_string(i) + to_string(j)+")";
                    wall.transform.translation = {i*0.5f, 0.0f, j*0.5f};
                    wall.transform.scale = {0.25f, 0.25f, 0.25f};
                    gameObjects.push_back(std::move(wall));
                }
            }
        }
    }
    static float canSolveCount = 0;
    static float cantSolveCount = 0;
    void MazeGameManager::Update(GLFWwindow *window) {
//        if(!SolveMaze()){cantSolveCount+=1; }
//        else canSolveCount+=1;
//        mazeRouteStack = {};
//        RefreshMaze();
//        CreateMaze();

        if(glfwGetKey(window, keys.resetMaze) == GLFW_PRESS && !onRefresh) {
            onRefresh = true;
            RefreshMaze();
            CreateMaze();
            //std::cout<<gameObjects.size()<<endl;
        }
        else if(glfwGetKey(window, keys.resetMaze) == GLFW_RELEASE && onRefresh) {
            onRefresh = false;
        }
        if(glfwGetKey(window, keys.solveMaze) == GLFW_PRESS && !canSolveMaze) {
            canSolveMaze = true;
            //std::cout<< canSolveCount<<"|"<<cantSolveCount<<"|"<<(float)canSolveCount/(cantSolveCount+canSolveCount)<<endl;
            if(!SolveMaze()){ std::cout<<"迷宫无解"<<endl;}
            mazeRouteStack = {};
        }else if(glfwGetKey(window, keys.solveMaze) == GLFW_RELEASE && canSolveMaze) {
            canSolveMaze = false;
        }
    }

    void MazeGameManager::RefreshMaze() {
        if(data != nullptr){
            free(data);
        }
        GenerateMaze();
    }



    bool MazeGameManager::SolveMaze() {
        if(mazeRouteStack.Size() == 0) {
            mazeRouteStack.Push({0,1});
        }
        MazePos curr = mazeRouteStack.Peek();
        //向左寻路
        if(data[ height * curr.x + curr.y+ 1 ] == 'o') {
            mazeRouteStack.Push({curr.x,curr.y+ 1});
            data[ height * (curr.x) + curr.y + 1 ] = 'z';
            if(SolveMaze()){
                AddRoad(curr.x, curr.y);
                return true;
            }
            mazeRouteStack.Pop();
        }
        //向前寻路
        if(data[ height * (curr.x + 1) + curr.y ] == 'o') {
            mazeRouteStack.Push({curr.x + 1,curr.y});
            data[ height * (curr.x + 1) + curr.y ] = 'z';
            if(SolveMaze()) {
                AddRoad(curr.x, curr.y);
                return true;
            }
            mazeRouteStack.Pop();
        }
        //向右寻路
        if(data[ height * curr.x + curr.y - 1 ] == 'o') {
            mazeRouteStack.Push({curr.x,curr.y - 1});
            data[ height * (curr.x) + curr.y - 1 ] = 'z';
            if(SolveMaze()) {
                AddRoad(curr.x, curr.y);
                return true;
            }
            mazeRouteStack.Pop();
        }
        //终点
        if(curr.x == width - 1 && curr.y == height - 2) {
            AddRoad(curr.x, curr.y);
            return true;
        }

        AddWall(curr.x,curr.y);
        return false;
    }

    void MazeGameManager::AddWall(int x, int y) {
        auto wall = LekGameObject::createGameObject();
        wall.model = wallModel;
        wall.name = "wall("+ to_string(x) + to_string(y)+")";
        wall.transform.translation = {x*0.5f, 0.0f, y*0.5f};
        wall.transform.scale = {0.25f, 0.25f, 0.25f};
        gameObjects.push_back(std::move(wall));
    }

    void MazeGameManager::AddRoad(int x, int y) {
        auto wall = LekGameObject::createGameObject();
        wall.model = roadModel;
        wall.name = "road("+ to_string(x) + to_string(y)+")";
        wall.transform.translation = {x*0.5f, 0.0f, y*0.5f};
        wall.transform.scale = {0.25f, 0.25f, 0.25f};
        gameObjects.push_back(std::move(wall));
    }

    void MazeRouteStack::Push(MazePos pos) {
        data.push_back(pos);
    }

    MazePos MazeRouteStack::Pop() {
        MazePos res = data.back();
        data.pop_back();
        return res;
    }

    MazePos MazeRouteStack::Peek() {
        return data.back();
    }

    int MazeRouteStack::Size() {
        return data.size();
    }
} // leking