#include "Engine_Pipeline.h"
#include "Engine_Device.h"
#include "vulkan/vulkan_core.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace engine{

EnginePipeline::EnginePipeline(
        class EngineDevice &device, 
        const PipelineConfigInfo &configInfo, 
        const std::string &vertFilepath, 
        const std::string &fragFilepath) : EngineDevice{device} {
    createGraphicsPipeline(configInfo, vertFilepath, fragFilepath);
}

std::vector<char> EnginePipeline::readFile(const std::string &filepath){
    std::ifstream file{filepath, std::ios::ate | std::ios::binary};

    if (!file.is_open()){
        throw std::runtime_error("failed to open file: " + filepath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    
    file.close();
    return buffer;
}

void EnginePipeline::createGraphicsPipeline(const PipelineConfigInfo &configInfo,const std::string &vertFilepath, const std::string &fragFilepath){
    auto vertCode = readFile(vertFilepath);
    auto fragCode = readFile(fragFilepath);

    std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
    std::cout << "Frag Shader Code Size: " << fragCode.size() << '\n';
}

void EnginePipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule){
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if(vkCreateShaderModule(EngineDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS){
        throw std::runtime_error("failed to create shader module.");
    }
}

PipelineConfigInfo EnginePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height){
    PipelineConfigInfo configInfo{};

    return configInfo;
}

} //namespace engine
