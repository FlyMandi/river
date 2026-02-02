#pragma once

#include "Engine_Device.h"
#include "vulkan/vulkan_core.h"

#include <cstdint>
#include <vector>
#include <string>

namespace engine{

struct PipelineConfigInfo{};

class EnginePipeline{
 public:
    EnginePipeline(EngineDevice &device, const PipelineConfigInfo &configInfo, const std::string &vertFilepath, const std::string &fragFilepath);

    ~EnginePipeline(){}

    EnginePipeline(const EnginePipeline&) = delete;
    EnginePipeline operator=(const EnginePipeline&) = delete;

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

 private:
    static std::vector<char> readFile(const std::string &filepath);

    void createGraphicsPipeline(const PipelineConfigInfo &configInfo,const std::string &vertFilepath, const std::string &fragFilepath);

    void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

    EngineDevice &EngineDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};

}  //namespace engine 
    

