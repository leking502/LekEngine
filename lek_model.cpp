//
// Created by leking on 22-9-30.
//

#include "lek_model.hpp"

#include <cassert>
#include <cstring>

namespace leking {
    LekModel::LekModel(LekDevice &device, const LekModel::Builder &builder) : lekDevice(device){
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    LekModel::~LekModel() {
        vkDestroyBuffer(lekDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(lekDevice.device(), vertexBufferMemory, nullptr);
        if(hasIndexBuffer) {
            vkDestroyBuffer(lekDevice.device(), indexBuffer, nullptr);
            vkFreeMemory(lekDevice.device(), indexBufferMemory, nullptr);
        }
    }

    void LekModel::createVertexBuffers(const vector<Vertex> &vertices) {
        vertexCount = static_cast<uint32_t >(vertices.size());
        assert(vertexCount >= 3 && "顶点点数量至少为3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        lekDevice.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                vertexBuffer,
                vertexBufferMemory);

        void *data;
        vkMapMemory(lekDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(lekDevice.device(), vertexBufferMemory);
    }

    void LekModel::createIndexBuffers(const vector<uint32_t> &indices) {
        indexCount = static_cast<uint32_t >(indices.size());
        hasIndexBuffer = indexCount > 0;

        if(!hasIndexBuffer) return;

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        lekDevice.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                indexBuffer,
                indexBufferMemory);

        void *data;
        vkMapMemory(lekDevice.device(), indexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(lekDevice.device(), indexBufferMemory);
    }

    void LekModel::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize  offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if(hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void LekModel::draw(VkCommandBuffer commandBuffer) {
        if(hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    vector<VkVertexInputBindingDescription> LekModel::Vertex::getBindingDescriptions() {
        vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride =sizeof (Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    vector<VkVertexInputAttributeDescription> LekModel::Vertex::getAttributeDescriptions() {
        vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex,position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex,color);
        return attributeDescriptions;
    }
} // leking