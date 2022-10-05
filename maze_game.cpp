//
// Created by leking on 22-10-1.
//

#include <cstring>
#include <iostream>
#include <chrono>
#include "maze_game.hpp"

#define OPENNESS 3

namespace leking {
    MazeGameManager::MazeGameManager(LekDevice& device, std::vector<LekGameObject>& gameObjects, int width, int height) : device(device), gameObjects(gameObjects), width(width), height(height){
        srand((unsigned)time(NULL));
        if(width < 3 || height < 3) {
            throw runtime_error("At least 3 in length or width");
        }
        wallModel = LekModel::createModelFromFile(device, "models/colored_cube.obj");
        roadModel = LekModel::createModelFromFile(device, "models/cube.obj");
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
                if(data[height*i+j] == 'x')
                {
                    AddWall(i, j);
                }
            }
        }
    }
    static float canSolveCount = 0;
    static float cantSolveCount = 0;
    void MazeGameManager::Update(GLFWwindow *window, float dt) {
//        if(!SolveMaze()){cantSolveCount+=1; }
//        else canSolveCount+=1;
//        mazeRouteStack = {};
//        RefreshMaze();
//        CreateMaze();

        if(glfwGetKey(window, keys.resetMaze) == GLFW_PRESS && !onRefresh && !autoMod) {
            onRefresh = true;
            RefreshMaze();
            CreateMaze();
            //std::cout<<gameObjects.size()<<endl;
        }
        else if(glfwGetKey(window, keys.resetMaze) == GLFW_RELEASE && onRefresh && !autoMod) {
            mazeRouteStack = {};
            solvedSuccessfully = false;
            startSolveMaze = false;
            onRefresh = false;
        }
        if(glfwGetKey(window, keys.autoSolveMaze) == GLFW_PRESS && !autoMod) {
            autoMod = true;
        }else if(glfwGetKey(window, keys.autoSolveMaze) == GLFW_RELEASE && autoMod) {
            autoMod = false;
        }
        if(autoMod){
            if(count>0.125f){
                SolveMaze();
                count = 0;
                return;
            }
            count += dt;
        }
        if(glfwGetKey(window, keys.solveMaze) == GLFW_PRESS && !canSolveMaze && !autoMod) {
            canSolveMaze = true;
            SolveMaze();
        }
        else if(glfwGetKey(window, keys.solveMaze) == GLFW_RELEASE && canSolveMaze && !autoMod) {
            canSolveMaze = false;
        }
    }

    void MazeGameManager::RefreshMaze() {
        if(data != nullptr){
            free(data);
        }
        GenerateMaze();
    }


    void MazeGameManager::SolveMaze() {
        if(solvedSuccessfully) return;
        if(mazeRouteStack.Size() == 0 && !startSolveMaze) {
            startSolveMaze = true;
            if(data[height*0+1] == 'z') {
                startSolveMaze = false;
                std::cout<<"no solve"<<endl;
                return;
            }
            mazeRouteStack.Push({0,1});
            AddWalkThrough(0,1);
            data[height*0+1] = 'z';
            return;
        }
        switch(StepMaze()){
            case 0:
                mazeRouteStack.Pop();
                PopWall();
                break;
            case 1:
                break;
            case 2:
                solvedSuccessfully = true;
                for(int i = 0;i<mazeRouteStack.Size();i++){
                    gameObjects.pop_back();
                }
                while (mazeRouteStack.Size() != 0) {
                    MazePos curr = mazeRouteStack.Pop();
                    AddRoad(curr.x,curr.y);
                }
                return;
        }
        if(mazeRouteStack.Size() == 0 && startSolveMaze) {
            mazeRouteStack = {};
            startSolveMaze = false;
            std::cout<<"no solve"<<endl;
        }
    }


    int MazeGameManager::StepMaze() {
        MazePos curr = mazeRouteStack.Peek();
        //终点
        if(curr.x == width - 1 && curr.y == height - 2) {
            return 2;
        }
        //向左寻路
        if(data[ height * curr.x + curr.y+ 1 ] == 'o') {
            mazeRouteStack.Push({curr.x,curr.y+ 1});
            data[ height * (curr.x) + curr.y + 1 ] = 'z';
            AddWalkThrough(curr.x,curr.y + 1);
            return 1;
        }
        //向前寻路
        if(data[ height * (curr.x + 1) + curr.y ] == 'o') {
            mazeRouteStack.Push({curr.x + 1,curr.y});
            data[ height * (curr.x + 1) + curr.y ] = 'z';
            AddWalkThrough(curr.x + 1,curr.y);
            return 1;
        }
        //向右寻路
        if(data[ height * curr.x + curr.y - 1 ] == 'o') {
            mazeRouteStack.Push({curr.x,curr.y - 1});
            data[ height * (curr.x) + curr.y - 1 ] = 'z';
            AddWalkThrough(curr.x,curr.y - 1);
            return 1;
        }
        //向后寻路
        if(data[ height * (curr.x - 1) + curr.y ] == 'o') {
            mazeRouteStack.Push({curr.x - 1,curr.y});
            data[ height * (curr.x - 1) + curr.y  ] = 'z';
            AddWalkThrough(curr.x - 1,curr.y);
            return 1;
        }
        return 0;
    }

    void MazeGameManager::AddWall(int x, int y) {
        auto wall = LekGameObject::createGameObject();
        wall.model = wallModel;
        wall.name = "wall("+ to_string(x) + to_string(y)+")";
        wall.transform.translation = {x, 0.0f, y};
        wall.transform.scale = {0.5f, 0.5f, 0.5f};
        gameObjects.push_back(std::move(wall));
    }
    void MazeGameManager::PopWall() {
        gameObjects.pop_back();
    }

    void MazeGameManager::AddWalkThrough(int x, int y) {
        auto wall = LekGameObject::createGameObject();
        wall.model = wallModel;
        wall.name = "wall("+ to_string(x) + to_string(y)+")";
        wall.transform.translation = {x, 0.0f, y};
        wall.transform.scale = {0.25f, 0.25f, 0.25f};
        gameObjects.push_back(std::move(wall));
    }

    void MazeGameManager::AddRoad(int x, int y) {
        auto wall = LekGameObject::createGameObject();
        wall.model = roadModel;
        wall.name = "road("+ to_string(x) + to_string(y)+")";
        wall.transform.translation = {x, 0.0f, y};
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