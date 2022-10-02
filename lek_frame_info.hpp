//
// Created by leking on 22-10-2.
//

#ifndef LEKENGINE_LEK_FRAME_INFO_HPP
#define LEKENGINE_LEK_FRAME_INFO_HPP

#include "lek_camera.hpp"

#include <vulkan/vulkan.h>

namespace leking {

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LekCamera& camera;
    };

} // leking

#endif //LEKENGINE_LEK_FRAME_INFO_HPP
