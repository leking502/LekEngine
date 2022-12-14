//
// Created by leking on 22-9-30.
//

#include "lek_model.hpp"

#include "lek_utils.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "ModelLoder/tiny_obj_loader.h"

#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
    template<>
    struct hash<leking::LekModel::Vertex> {
        size_t operator()(leking::LekModel::Vertex const &vertex) const {
            size_t seed = 0;
            leking::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };

}

namespace leking {
    LekModel::LekModel(LekDevice &device, const LekModel::Builder &builder) : lekDevice(device){
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }
    LekModel::LekModel(LekDevice &device, const LekModel::Builder2D &builder) : lekDevice(device){
        createVertex2DBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    LekModel::~LekModel() {}

    std::unique_ptr<LekModel> LekModel::createModelFromFile(LekDevice &device, const string &filePath) {
        Builder builder{};
        builder.loadModel(filePath);
        return std::make_unique<LekModel>(device, builder);
    }

    void LekModel::createVertexBuffers(const vector<Vertex> &vertices) {
        vertexCount = static_cast<uint32_t >(vertices.size());
        assert(vertexCount >= 3 && "The number of vertices shall be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        LekBuffer stagingBuffer {
            lekDevice,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)vertices.data());

        vertexBuffer = std::make_unique<LekBuffer>(
                lekDevice,
                vertexSize,
                vertexCount,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        lekDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }
    void LekModel::createVertex2DBuffers(const vector<Vertex2D> &vertices) {
        vertexCount = static_cast<uint32_t >(vertices.size());
        assert(vertexCount >= 3 && "The number of vertices shall be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        LekBuffer stagingBuffer {
                lekDevice,
                vertexSize,
                vertexCount,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)vertices.data());

        vertexBuffer = std::make_unique<LekBuffer>(
                lekDevice,
                vertexSize,
                vertexCount,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        lekDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void LekModel::createIndexBuffers(const vector<uint32_t> &indices) {
        indexCount = static_cast<uint32_t >(indices.size());
        hasIndexBuffer = indexCount > 0;

        if(!hasIndexBuffer) return;

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        uint32_t indexSize = sizeof(indices[0]);

        LekBuffer stagingBuffer {
            lekDevice,
            indexSize,
            indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)indices.data());

        indexBuffer = std::make_unique<LekBuffer>(
                lekDevice,
                indexSize,
                indexCount,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


        lekDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);

    }

    void LekModel::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer->getBuffer()};
        VkDeviceSize  offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if(hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
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
        vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({
            0,
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex,position)});
        attributeDescriptions.push_back({
            1,
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex,color)});
        attributeDescriptions.push_back({
            2,
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex,normal)});
        attributeDescriptions.push_back({
            3,
            0,
            VK_FORMAT_R32G32_SFLOAT,
            offsetof(Vertex,uv)});


        return attributeDescriptions;
    }

    vector<VkVertexInputBindingDescription> LekModel::Vertex2D::getBindingDescriptions() {
        vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride =sizeof (Vertex2D);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    vector<VkVertexInputAttributeDescription> LekModel::Vertex2D::getAttributeDescriptions() {
        vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({
                                                0,
                                                0,
                                                VK_FORMAT_R32G32_SFLOAT,
                                                offsetof(Vertex2D,position)});
        attributeDescriptions.push_back({
                                                1,
                                                0,
                                                VK_FORMAT_R32G32B32_SFLOAT,
                                                offsetof(Vertex2D,color)});


        return attributeDescriptions;
    }

    void LekModel::Builder::loadModel(const string &filePath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string  warn, err;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
            throw  std::runtime_error(warn+err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for(const auto &shape : shapes) {
            for(const auto &index : shape.mesh.indices) {
                Vertex vertex{};
                if(index.vertex_index >= 0) {
                    vertex.position = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.color = {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if(index.normal_index >= 0) {
                    vertex.normal = {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if(index.texcoord_index >= 0) {
                    vertex.uv = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                if( uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }

    }
} // leking