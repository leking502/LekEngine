//
// Created by leking on 22-9-29.
//

#ifndef LEKENGINE_LEK_PIPELINE_HPP
#define LEKENGINE_LEK_PIPELINE_HPP

#include <string>
#include <vector>
#include "lek_device.hpp"

using namespace std;

struct PipelineConfigInfo {
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};
namespace leking {
    class LekPipeline {
    public:
        LekPipeline(
                LekDevice& device,
                const string &vertFilePath,
                const string &fragFilePath,
                const PipelineConfigInfo& configInfo);

        ~LekPipeline();

        LekPipeline(const LekPipeline&) = delete;
        LekPipeline& operator=(const LekPipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        static vector<char> readFile(const string& filePath);

        void createGraphicsPipeline(const string& vertFilePath,
                                    const string& fragFilePath,
                                    const PipelineConfigInfo& configInfo);

        void createShaderModule(const vector<char>& code, VkShaderModule* shaderModule);

        LekDevice& lekDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}

#endif //LEKENGINE_LEK_PIPELINE_HPP
