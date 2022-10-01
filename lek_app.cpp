//
// Created by leking on 22-9-28.
//

#include "lek_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lek_camera.hpp"
#include "simple_render_system.hpp"
#include "maze_game.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#define MAX_FRAME_TIME 1.0f
#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>

using namespace std;
using namespace glm;
namespace leking {

    LekApp::LekApp() {
        //loadGameObjects();
    }


    LekApp::~LekApp() {}

    void leking::LekApp::run() {
        SimpleRenderSystem simpleRenderSystem(lekDevice, lekRenderer.getSwapChainRenderPass());
        LekCamera camera{};

        auto viewerObject = LekGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        MazeGameManager mazeGameManager = MazeGameManager(10,10);
        mazeGameManager.CreateMaze(lekDevice,gameObjects);

        while(!lekWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTine = std::chrono::duration<float, std::chrono::seconds::period>(newTime-currentTime).count();
            currentTime = newTime;

            frameTine = glm::min(frameTine,MAX_FRAME_TIME);

            mazeGameManager.Update(lekDevice, lekWindow.getGLFWwindow(),gameObjects);

            cameraController.moveInPlaneXZ(lekWindow.getGLFWwindow(), frameTine, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lekRenderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

            if(auto  commandBuffer = lekRenderer.beginFrame()) {

                lekRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                lekRenderer.endSwapChainRenderPass(commandBuffer);
                lekRenderer.endFrame();
            }

        }
        vkDeviceWaitIdle(lekDevice.device());
    }

    std::unique_ptr<LekModel> createCubeModel(LekDevice& device, glm::vec3 offset) {
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

    void LekApp::loadGameObjects() {
        std::shared_ptr<LekModel> lekModel = createCubeModel(lekDevice, {0.0f, 0.0f, 0.0f});

        for(int i = 0;i<1;i++){
            auto cube = LekGameObject::createGameObject();
            cube.model = lekModel;
            cube.transform.translation = {0.0f, 0.0f, 2.5f};
            cube.transform.scale = {0.5f, 0.5f, 0.5f};
            gameObjects.push_back(std::move(cube));
        }
    }

}

