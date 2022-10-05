//
// Created by leking on 22-10-1.
//

#ifndef LEKENGINE_LEK_RENDERER_HPP
#define LEKENGINE_LEK_RENDERER_HPP

#include "lek_window.hpp"
#include "lek_device.hpp"
#include "lek_swap_chain.hpp"
#include "lek_model.hpp"

#include <cassert>
#include <memory>
#include <vector>

using namespace std;
namespace leking {
    class LekRenderer {
    public:

        LekRenderer(LekDevice& device, LekWindow& window);
        ~LekRenderer();

        LekRenderer(const LekRenderer &) = delete;
        LekRenderer& operator=(const LekRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const {return lekSwapChain->getRenderPass();}
        float getAspectRatio() const { return lekSwapChain->extentAspectRatio();}
        bool isFrameInProgress() const {return  isFrameStarted;}

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame is not in program");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "The frame index cannot be obtained when the frame is not in the program");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        LekWindow& lekWindow;
        LekDevice& lekDevice;
        unique_ptr<LekSwapChain> lekSwapChain;
        vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};

    };
}
#endif //LEKENGINE_LEK_RENDERER_HPP
