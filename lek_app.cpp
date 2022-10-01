//
// Created by leking on 22-9-28.
//

#include "lek_app.hpp"

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

using namespace std;
using namespace glm;
namespace leking {

    LekApp::LekApp() {
        loadGameObjects();
    }


    LekApp::~LekApp() {}

    void leking::LekApp::run() {
        SimpleRenderSystem simpleRenderSystem(lekDevice, lekRenderer.getSwapChainRenderPass());


        while(!lekWindow.shouldClose()) {
            glfwPollEvents();

            if(auto  commandBuffer = lekRenderer.beginFrame()) {

                lekRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                lekRenderer.endSwapChainRenderPass(commandBuffer);
                lekRenderer.endFrame();
            }

        }
        vkDeviceWaitIdle(lekDevice.device());
    }

    void LekApp::loadGameObjects() {
        vector<LekModel::Vertex> vertices {
                {{0.0f, -0.5f},{1.0,0.0,0.0}},
                {{0.5f, 0.5f},{0.0,1.0,0.0}},
                {{-0.5f, 0.5f},{0.0,0.0,1.0}}
        };
        auto lekModel = std::make_shared<LekModel>(lekDevice, vertices);

        for(int i = 0;i<40;i++) {
            for (int j = 0; j < 40; ++j) {
                auto triangle = LekGameObject::createGameObject();
                triangle.model = lekModel;
                triangle.color = {1.0f, 1.0f, 1.0f};
                triangle.transform2d.translation.x = -1 + 0.05f*i;
                triangle.transform2d.translation.y = -1 + 0.05*j;
                triangle.transform2d.scale = {0.01f, 0.05f};

                gameObjects.push_back(std::move(triangle));
            }
        }
        for(int i = 1;i<3;i++) {
            auto triangle = LekGameObject::createGameObject();
            triangle.model = lekModel;
            triangle.color = {1.0f, i-1, 1.0f};
            triangle.transform2d.translation.x = 0;
            triangle.transform2d.translation.y = 0;
            triangle.transform2d.scale = {0.1f, 0.1f};
            triangle.test = i;

            gameObjects.push_back(std::move(triangle));
        }
    }

}

