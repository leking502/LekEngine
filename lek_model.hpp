//
// Created by leking on 22-9-30.
//

#ifndef LEKENGINE_LEK_MODEL_HPP
#define LEKENGINE_LEK_MODEL_HPP

#include "lek_device.hpp"
#include "lek_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include <memory>
#include <vector>

using namespace glm;
using namespace std;

namespace leking {

    class LekModel {
    public:

        struct Vertex {
            vec3 position{};
            vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const {
                return position == other.position
                && color == other.color
                && normal == other.normal
                && uv == other.uv;
            }
        };
        struct Vertex2D {
            vec2 position{};
            vec3 color{};

            static vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t > indices{};

            void loadModel(const std::string& filePath);
        };
        struct Builder2D {
            std::vector<Vertex2D> vertices{};
            std::vector<uint32_t > indices{};
        };

        LekModel(LekDevice &device, const LekModel::Builder &builder);
        LekModel(LekDevice &device, const LekModel::Builder2D &builder);
        ~LekModel();

        LekModel(const LekModel &) = delete;
        LekModel &operator=(const LekModel &) = delete;

        static std::unique_ptr<LekModel> createModelFromFile(LekDevice& device, const std::string& filePath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertex2DBuffers(const vector<Vertex2D> &vertices);
        void createVertexBuffers(const vector<Vertex> &vertices);
        void createIndexBuffers(const vector<uint32_t> &indices);

        LekDevice& lekDevice;

        std::unique_ptr<LekBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<LekBuffer> indexBuffer;
        uint32_t indexCount;

    };

} // leking

#endif //LEKENGINE_LEK_MODEL_HPP
