//
// Created by leking on 22-9-28.
//

#include "lek_app.hpp"
#include "lek_buffer.hpp"

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
#include <csignal>

using namespace std;
using namespace glm;
namespace leking {

    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.0f, -1.0f});
    };

    LekApp::LekApp() {
        //loadGameObjects();
    }


    LekApp::~LekApp() {}

    void leking::LekApp::run() {

        std::vector<std::unique_ptr<LekBuffer>> uboBuffers(LekSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i< uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<LekBuffer>(
                lekDevice,
                sizeof(GlobalUbo),
                LekSwapChain::MAX_FRAMES_IN_FLIGHT,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        SimpleRenderSystem simpleRenderSystem(lekDevice, lekRenderer.getSwapChainRenderPass());
        LekCamera camera{};

        auto viewerObject = LekGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        MazeGameManager mazeGameManager = MazeGameManager(lekDevice, gameObjects,30,30);
        mazeGameManager.CreateMaze();


        shared_ptr<LekModel>  vaseModel = LekModel::createModelFromFile(lekDevice, "/home/leking/CLionProjects/LekEngine/models/oo.obj");
        auto vase = LekGameObject::createGameObject();
        vase.model = vaseModel;
        vase.transform.translation = {0, -1.0f, 1};
        vase.transform.rotation = {glm::pi<float>(),glm::half_pi<float>(),0};
        vase.transform.scale = {0.1f, 0.1f, 0.1f};
        gameObjects.push_back(std::move(vase));

        while(!lekWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTine = std::chrono::duration<float, std::chrono::seconds::period>(newTime-currentTime).count();
            currentTime = newTime;

            frameTine = glm::min(frameTine,MAX_FRAME_TIME);

            mazeGameManager.Update(lekWindow.getGLFWwindow());

            cameraController.moveInPlaneXZ(lekWindow.getGLFWwindow(), frameTine, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lekRenderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 40000.0f);

            if(auto  commandBuffer = lekRenderer.beginFrame()) {

                int frameIndex = lekRenderer.getFrameIndex();
                FrameInfo frameInfo {
                    frameIndex,
                    frameTine,
                    commandBuffer,
                    camera
                };
                //更新
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //渲染
                lekRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                lekRenderer.endSwapChainRenderPass(commandBuffer);
                lekRenderer.endFrame();
            }
            LekGameObject::GameObjectGC(gameObjects);

        }
        vkDeviceWaitIdle(lekDevice.device());
    }

    void LekApp::loadGameObjects() {
        std::shared_ptr<LekModel> lekModel = LekModel::createModelFromFile(lekDevice, "/home/leking/CLionProjects/LekEngine/models/colored_cube.obj");

        for(int i = 0;i<1;i++){
            auto cube = LekGameObject::createGameObject();
            cube.model = lekModel;
            cube.transform.translation = {0.0f, 0.0f, 2.5f};
            cube.transform.scale = {0.5f, 0.5f, 0.5f};
            gameObjects.push_back(std::move(cube));
        }
    }

}

