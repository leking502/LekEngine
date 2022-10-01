//
// Created by leking on 22-10-1.
//

#ifndef LEKENGINE_SIMPLE_RENDER_SYSTEM_HPP
#define LEKENGINE_SIMPLE_RENDER_SYSTEM_HPP

#include "lek_camera.hpp"
#include "lek_pipeline.hpp"
#include "lek_game_object.hpp"
#include "lek_device.hpp"

#include <memory>
#include <vector>

namespace leking {

    class SimpleRenderSystem {
    public:

        SimpleRenderSystem(LekDevice& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(
                VkCommandBuffer commandBuffer,
                std::vector<LekGameObject>& gameObjects,
                const LekCamera& camera);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        LekDevice& lekDevice;

        unique_ptr<LekPipeline> lekPipeline;
        VkPipelineLayout  pipelineLayout;
    };

} // leking

#endif //LEKENGINE_SIMPLE_RENDER_SYSTEM_HPP
