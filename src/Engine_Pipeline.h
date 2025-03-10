#pragma once

#include "Engine_Device.h"
#include "vulkan/vulkan_core.h"

#include <cstdint>
#include <vector>
#include <string>

namespace engine{

struct PipelineConfigInfo{
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo operator=(const PipelineConfigInfo&) = delete;

    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class EnginePipeline{
    EngineDevice &EngineDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    static std::vector<char> readFile(const std::string &filepath);

    void createGraphicsPipeline(const PipelineConfigInfo &configInfo, const std::string &vertFilepath, const std::string &fragFilepath);
    VkShaderModule createShaderModule(const std::vector<char> &code);

 public:
    EnginePipeline(class EngineDevice &device, const PipelineConfigInfo &configInfo, const std::string &vertFilepath, const std::string &fragFilepath);
    ~EnginePipeline();

    EnginePipeline(const EnginePipeline&) = delete;
    EnginePipeline operator=(const EnginePipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);

    static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo, uint32_t width, uint32_t height);
};

}  //namespace engine 
