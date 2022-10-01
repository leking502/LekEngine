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
        static bool start = false;
        static glm::vec2 targetPos;
        static glm::vec2 targetPos1;
        static glm::vec2 v;
        static glm::vec2 v1;
        static glm::vec2 a;
        static glm::vec2 a1;
        if(!start) {
            start = true;
            targetPos = {0.3f, 0.3f};
            targetPos1 = {-0.2f, -0.3f};
            v = {1.f,0.0f};
            v1 = {-1.f,0.0f};
            a = {0.0f,0.0f};
            a1 = {0.0f,0.0f};
        }
        for(auto&obj : gameObjects) {
            if(obj.test == 1) {
                obj.transform2d.translation  = targetPos;
                continue;
            }
            if(obj.test == 2) {
                obj.transform2d.translation  = targetPos1;
                continue;
            }
            glm::vec2 d =  targetPos - obj.transform2d.translation;
            float dlp =  glm::dot(d,d);
            glm::vec2 d1 =  targetPos1 - obj.transform2d.translation;
            float d1lp =  glm::dot(d1,d1);
            glm::vec2 dan = {d.x/dlp,d.y/dlp};
            glm::vec2 da1n = {d1.x/d1lp,d1.y/d1lp};
            glm::vec2 da = normalize(dan+da1n);
            obj.transform2d.rotation = glm::acos(da.x) + glm::half_pi<float>();

            targetPos += v*0.00001f;
            targetPos1 += v1*0.00001f;

            glm::vec2 r = targetPos1 - targetPos;
            float rd = glm::dot(r,r);
            a = {r.x/rd,r.y/rd};
            glm::vec2 r1 = targetPos - targetPos1;
            float rd1 = glm::dot(r,r);
            a1 = {r1.x/rd1,r1.y/rd1};

            v = {v.x+a.x*0.00001f,v.y+a.y*0.00001f};
            v1 = {v1.x+a1.x*0.00001f,v1.y+a1.y*0.00001f};

            std::cout<< a.x<<endl;
            std::cout<< v.x<<endl;
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