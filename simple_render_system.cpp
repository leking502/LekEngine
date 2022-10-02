//
// Created by leking on 22-10-1.
//

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <array>
#include <cassert>
#include <stdexcept>
#include <iostream>

namespace leking {
    struct SimplePushConstantData2D {
        mat2 transform{1.0f};
        vec2 offset;
        alignas(16) vec3 color;
    };
    struct SimplePushConstantData {
        glm::mat4 transform{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };

    SimpleRenderSystem::SimpleRenderSystem(LekDevice& device, VkRenderPass renderPass) : lekDevice(device){
        createPipelineLayout();
        createPipeline(renderPass);
    }


    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(lekDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if(vkCreatePipelineLayout(lekDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw runtime_error("创建管道层失败");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "不能在创建管线层之前创建渲染管线");

        PipelineConfigInfo pipelineConfig{};
        LekPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lekPipeline = make_unique<LekPipeline>(
                lekDevice,
                "/home/leking/CLionProjects/LekEngine/shaders/simple_shader.vert.spv",
                "/home/leking/CLionProjects/LekEngine/shaders/simple_shader.frag.spv",
                pipelineConfig);
    }


    void SimpleRenderSystem::renderGameObjects(
            FrameInfo& frameInfo,
            std::vector<LekGameObject>& gameObjects) {


        lekPipeline->bind(frameInfo.commandBuffer);

        auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

        //渲染游戏对象
        for(auto& obj: gameObjects) {

            if(!obj.CanDraw()) continue;

            SimplePushConstantData push{};
            auto modelMatrix = obj.transform.mat4();
            push.transform = projectionView * obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                    frameInfo.commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(SimplePushConstantData),
                    &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }
} // leking