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
            vec3 position;
            vec3 color;

            static vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
        struct Vertex2D {
            vec2 position;
            vec3 color;

            static vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t > indices{};
        };

        LekModel(LekDevice &device, const LekModel::Builder &builder);
        ~LekModel();

        LekModel(const LekModel &) = delete;
        LekModel &operator=(const LekModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const vector<Vertex> &vertices);
        void createIndexBuffers(const vector<uint32_t> &indices);

        LekDevice& lekDevice;

        VkBuffer  vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        VkBuffer  indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };

} // leking

#endif //LEKENGINE_LEK_MODEL_HPP
