#include "Cnake.h"
#include "Engine_Pipeline.h"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>

namespace engine{

Cnake::Cnake(){
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

Cnake::~Cnake(){
    vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
}

void Cnake::run(){
    while(!engineWindow.shouldClose()){
        glfwPollEvents();
    }
}

void Cnake::createPipelineLayout(){
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if(vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("failed to create pipeline layout.");        
    }
}

void Cnake::createPipeline(){
    PipelineConfigInfo pipelineConfig{};
    EnginePipeline::defaultPipelineConfigInfo(pipelineConfig, engineSwapChain.width(), engineSwapChain.height());
    pipelineConfig.renderPass = engineSwapChain.getRenderPass(); 
    pipelineConfig.pipelineLayout = pipelineLayout; 
    enginePipeline = std::make_unique<EnginePipeline>(engineDevice, pipelineConfig, "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv");
}

void Cnake::createCommandBuffers(){}
void Cnake::drawFrame(){}

} //namespace engine
