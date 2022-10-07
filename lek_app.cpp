//
// Created by leking on 22-9-28.
//

#include "lek_app.hpp"
#include "lek_buffer.hpp"

#include "keyboard_movement_controller.hpp"
#include "lek_camera.hpp"
#include "simple_render_system.hpp"
#include "maze_game.hpp"
#include "ui_render_system.hpp"

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
#include <iostream>

using namespace std;
using namespace glm;
namespace leking {

    struct GlobalUbo {
        alignas(16) glm::mat4 projectionView{1.f};
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.0f, -1.0f});
    };

    LekApp::LekApp() {
        globalPool = LekDescriptorPool::Builder(lekDevice)
                .setMaxSets(LekSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,LekSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        //loadGameObjects();
    }


    LekApp::~LekApp() {}

    void leking::LekApp::run(int mw, int mh) {

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

        auto globalSetLayout =
                LekDescriptorSetLayout::Builder(lekDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LekSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LekDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0,&bufferInfo)
            .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem(
                lekDevice,
                lekRenderer.getSwapChainRenderPass(),
                globalSetLayout->getDescriptorSetLayout());
        UIRenderSystem uiRenderSystem(lekDevice, lekRenderer.getSwapChainRenderPass());
        LekCamera camera{};

        auto viewerObject = LekGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        //UI
        LekModel::Builder2D builder{
                {
                        {{-1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},
                        {{1.0f, -1.5f}, {1.0f, 1.0f, 1.0f}},
                        {{1.0f, -0.8f}, {1.0f, 1.0f, 1.0f}},
                        {{-1.0f, -0.8f}, {1.0f, 1.0f, 1.0f}}},
                {0,1,2,2,3,0}};

        auto lveModel = std::make_shared<LekModel>(lekDevice, builder);

        auto triangle = LekGameObject2D::createGameObject();
        triangle.model = lveModel;
        triangle.color = {1.f, 1.f, 1.f};
        //triangle.transform2d.translation.x = .2f;
        //triangle.transform2d.scale = {2.f, .5f};
        //triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects2D.push_back(std::move(triangle));


        MazeGameManager mazeGameManager = MazeGameManager(lekDevice, gameObjects,mw,mh);
        mazeGameManager.CreateMaze();

        viewerObject.transform.translation = { mw/2.f,-1.2*glm::max(mw,mh),(mh/2.f)-4.f };
        viewerObject.transform.rotation = { -glm::half_pi<float>(),0.0f,0.0f};

        while(!lekWindow.shouldClose()) {
            glfwPollEvents();

            //获取每帧时间
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            mazeGameManager.Update(lekWindow.getGLFWwindow(), frameTime);

            cameraController.moveInPlaneXZ(lekWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lekRenderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 40000.0f);

            if(auto  commandBuffer = lekRenderer.beginFrame()) {

                int frameIndex = lekRenderer.getFrameIndex();
                FrameInfo frameInfo {
                        frameIndex,
                        frameTime,
                        commandBuffer,
                        camera,
                        globalDescriptorSets[frameIndex]
                };
                //更新
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //渲染
                lekRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                uiRenderSystem.renderUI(frameInfo, gameObjects2D);
                lekRenderer.endSwapChainRenderPass(commandBuffer);
                lekRenderer.endFrame();
            }
            LekGameObject::GameObjectGC(gameObjects);

        }
        vkDeviceWaitIdle(lekDevice.device());
    }

    void LekApp::loadGameObjects() {
        std::shared_ptr<LekModel> lekModel = LekModel::createModelFromFile(lekDevice, "models/colored_cube.obj");

        for(int i = 0;i<1;i++){
            auto cube = LekGameObject::createGameObject();
            cube.model = lekModel;
            cube.transform.translation = {0.0f, 0.0f, 2.5f};
            cube.transform.scale = {0.5f, 0.5f, 0.5f};
            gameObjects.push_back(std::move(cube));
        }
    }

}

