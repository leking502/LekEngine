//
// Created by leking on 22-9-30.
//

#ifndef LEKENGINE_LEK_MODEL_HPP
#define LEKENGINE_LEK_MODEL_HPP

#include "lek_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include <vector>

using namespace glm;
using namespace std;

namespace leking {

    class LekModel {
    public:

        struct Vertex {
            vec2 position;
            vec3 color;

            static vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        LekModel(LekDevice &device, const vector<Vertex> &vertices);
        ~LekModel();

        LekModel(const LekModel &) = delete;
        LekModel &operator=(const LekModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const vector<Vertex> &vertices);

        LekDevice& lekDevice;
        VkBuffer  vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    };

} // leking

#endif //LEKENGINE_LEK_MODEL_HPP
