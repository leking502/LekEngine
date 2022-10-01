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
        bool isFrameInProgress() const {return  isFrameStarted;}

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "不能在帧不在程序内的时候获得命令缓冲区");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "不可以在帧不在程序的情况下获取帧索引");
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
