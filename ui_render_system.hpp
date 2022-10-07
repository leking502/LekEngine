//
// Created by leking on 22-10-6.
//

#ifndef LEKENGINE_UI_RENDER_SYSTEM_HPP
#define LEKENGINE_UI_RENDER_SYSTEM_HPP

#include "lek_camera.hpp"
#include "lek_pipeline.hpp"
#include "lek_game_object.hpp"
#include "lek_device.hpp"
#include "lek_frame_info.hpp"

#include <memory>
#include <vector>

namespace leking {

    class UIRenderSystem {
    public:

        UIRenderSystem(LekDevice& device, VkRenderPass renderPass);
        ~UIRenderSystem();

        UIRenderSystem(const UIRenderSystem &) = delete;
        UIRenderSystem& operator=(const UIRenderSystem&) = delete;

        void renderUI(
                FrameInfo& frameInfo,
                std::vector<LekGameObject2D>& gameObjects);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        LekDevice& lekDevice;

        unique_ptr<LekPipeline> lekPipeline;
        VkPipelineLayout  pipelineLayout;
    };

} // leking

#endif //LEKENGINE_UI_RENDER_SYSTEM_HPP
