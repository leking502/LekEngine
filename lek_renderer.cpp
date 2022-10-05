//
// Created by leking on 22-10-1.
//

#include "lek_renderer.hpp"


#include <array>
#include <cassert>
#include <stdexcept>

namespace leking {

    LekRenderer::LekRenderer(LekDevice& device, LekWindow& window) : lekDevice(device), lekWindow(window){
        recreateSwapChain();
        createCommandBuffers();
    }


    LekRenderer::~LekRenderer() {
        freeCommandBuffers();
    }


    void LekRenderer::recreateSwapChain() {
        auto extent = lekWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = lekWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(lekDevice.device());
        if(lekSwapChain == nullptr) {
            lekSwapChain = make_unique<LekSwapChain>(lekDevice, extent);
        } else {
            std::shared_ptr<LekSwapChain> oldSwapChain = std::move(lekSwapChain);
            lekSwapChain = make_unique<LekSwapChain>(lekDevice, extent, oldSwapChain);

            if(!oldSwapChain->compareSwapFormats(*lekSwapChain.get())) {
                throw std::runtime_error("交换链图像或深度格式发生变化");
            }
        }

        //如果渲染通道兼容的话可以不创建新的Pipeline
    }

    void LekRenderer::createCommandBuffers() {
        commandBuffers.resize(LekSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lekDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t >(commandBuffers.size());

        if(vkAllocateCommandBuffers(lekDevice.device(), &allocInfo, commandBuffers.data()) !=  VK_SUCCESS) {
            throw runtime_error("分配命令缓冲区失败");
        }
    }


    void LekRenderer::freeCommandBuffers() {;
        vkFreeCommandBuffers(
                lekDevice.device(),
                lekDevice.getCommandPool(),
                commandBuffers.size(),
                commandBuffers.data());
        commandBuffers.clear();
    }


    VkCommandBuffer LekRenderer::beginFrame() {
        assert(!isFrameStarted && "不可以在帧已经在程序的情况下调用开始帧");

        auto result = lekSwapChain->acquireNextImage(&currentImageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw runtime_error("无法获得交换链图像");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw runtime_error("开始记录命令缓冲区失败");
        }
        return commandBuffer;
    }

    void LekRenderer::endFrame() {
        assert(isFrameStarted && "不可以在帧不在程序的时候调用结束帧");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw runtime_error("记录命令缓冲区失败");
        }
        auto result = lekSwapChain->submitCommandBuffers(&commandBuffer ,&currentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lekWindow.wasWindowResized()) {
            lekWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw runtime_error("提交交换链图像失败");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % LekSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void LekRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "不可以在帧不在程序的时候调用开始交换链渲染通道");
        assert(commandBuffer == getCurrentCommandBuffer() &&"开始渲染不可以在不同的帧上传递给命令缓冲区");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lekSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lekSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lekSwapChain->getSwapChainExtent();

        array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.3f, 0.3f, 0.3f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lekSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lekSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, lekSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void LekRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "You cannot call the end exchange chain rendering channel when the frame is not in the program");
        assert(commandBuffer == getCurrentCommandBuffer() &&"End rendering cannot be passed to the command buffer on different frames");
        vkCmdEndRenderPass(commandBuffer);
    }
} // leking