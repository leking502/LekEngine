//
// Created by leking on 22-10-6.
//

#include "ui_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include <array>
#include <cassert>
#include <stdexcept>
#include <iostream>

namespace leking {
    struct UIPushConstantData {
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        alignas(16) vec3 color;
    };

    UIRenderSystem::UIRenderSystem(LekDevice& device, VkRenderPass renderPass) : lekDevice(device){
        createPipelineLayout();
        createPipeline(renderPass);
    }


    UIRenderSystem::~UIRenderSystem() {
        vkDestroyPipelineLayout(lekDevice.device(), pipelineLayout, nullptr);
    }

    void UIRenderSystem::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(UIPushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if(vkCreatePipelineLayout(lekDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw runtime_error("Failed to create pipeline layer");
        }
    }

    void UIRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "You cannot create a render pipeline before creating a pipeline layer");

        PipelineConfigInfo pipelineConfig{};
        LekPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lekPipeline = make_unique<LekPipeline>(
                lekDevice,
                PipeLineType::is_2d,
                "shaders/ui_shader.vert.spv",
                "shaders/ui_shader.frag.spv",
                pipelineConfig);
    }


    void UIRenderSystem::renderUI(
            FrameInfo& frameInfo,
            std::vector<LekGameObject2D>& gameObjects) {


        lekPipeline->bind(frameInfo.commandBuffer);

        //渲染游戏对象
        for(auto& obj: gameObjects) {

            if(!obj.CanDraw()) continue;

            UIPushConstantData push{};
            push.transform = obj.transform2d.mat2();
            push.offset = obj.transform2d.translation;
            push.color = obj.color;

            vkCmdPushConstants(
                    frameInfo.commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(UIPushConstantData),
                    &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }
} // leking