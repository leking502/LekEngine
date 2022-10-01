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

namespace leking {
    struct SimplePushConstantData {
        mat2 transform{1.f};
        vec2 offset;
        alignas(16) vec3 color;
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

    void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LekGameObject>& gameObjects) {
        //更新游戏对象
        static float speedx = 0.0f;
        static float addSpeedx = 0;
        static float speedy = 4.0f;
        static float addSpeedy = 0;
        int i =0;
        for(auto&obj : gameObjects) {
            i+=1;
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.00003f*i, glm::two_pi<float>());
            obj.transform2d.translation.x += speedx*0.0001f;
            obj.transform2d.translation.y += speedy*0.0001f;
            speedx += addSpeedx*0.0001f;
            addSpeedx = -obj.transform2d.translation.x;
            speedy += addSpeedy*0.0001f;
            addSpeedy = -obj.transform2d.translation.y;
        }

        //渲染游戏对象
        lekPipeline->bind(commandBuffer);
        for(auto& obj: gameObjects) {
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                    commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(SimplePushConstantData),
                    &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
} // leking