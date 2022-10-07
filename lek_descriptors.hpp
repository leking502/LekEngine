//
// Created by leking on 22-10-5.
//

#ifndef LEKENGINE_LEK_DESCRIPTORS_HPP
#define LEKENGINE_LEK_DESCRIPTORS_HPP


#include "lek_device.hpp"

#include <memory>
#include <unordered_map>
#include <vector>


namespace leking {
    class LekDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(LekDevice &lekDevice) : lekDevice{lekDevice} {}

            Builder &addBinding(
                    uint32_t binding,
                    VkDescriptorType descriptorType,
                    VkShaderStageFlags stageFlags,
                    uint32_t count = 1);
            std::unique_ptr<LekDescriptorSetLayout> build() const;

        private:
            LekDevice &lekDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        LekDescriptorSetLayout(
                LekDevice &lekDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~LekDescriptorSetLayout();
        LekDescriptorSetLayout(const LekDescriptorSetLayout &) = delete;
        LekDescriptorSetLayout &operator=(const LekDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        LekDevice &lekDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class LekDescriptorWriter;
    };

    class LekDescriptorPool {
    public:
        class Builder {
        public:
            Builder(LekDevice &lekDevice) : lekDevice{lekDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<LekDescriptorPool> build() const;

        private:
            LekDevice &lekDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        LekDescriptorPool(
                LekDevice &lekDevice,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~LekDescriptorPool();
        LekDescriptorPool(const LekDescriptorPool &) = delete;
        LekDescriptorPool &operator=(const LekDescriptorPool &) = delete;

        bool allocateDescriptor(
                const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        LekDevice &lekDevice;
        VkDescriptorPool descriptorPool;

        friend class LekDescriptorWriter;
    };

    class LekDescriptorWriter {
    public:
        LekDescriptorWriter(LekDescriptorSetLayout &setLayout, LekDescriptorPool &pool);

        LekDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        LekDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        LekDescriptorSetLayout &setLayout;
        LekDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // leking

#endif //LEKENGINE_LEK_DESCRIPTORS_HPP
