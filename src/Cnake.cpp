#include "Cnake.h"
#include "Engine_Pipeline.h"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>
#include <array>
#include <format>
#include <filesystem>

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
        drawFrame();
    }

    vkDeviceWaitIdle(engineDevice.device());
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

    std::string vert = "../src/shaders/simple_shader.vert.spv";
    std::string frag = "../src/shaders/simple_shader.frag.spv";
    if(!std::filesystem::exists(vert) || !std::filesystem::exists(frag)){
        throw std::runtime_error("could not find the necessary shaders for pipeline construction.");
    }

    enginePipeline = std::make_unique<EnginePipeline>(engineDevice, pipelineConfig, vert, frag); 
}

void Cnake::createCommandBuffers(){
    commandBuffers.resize(engineSwapChain.imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = engineDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if(vkAllocateCommandBuffers(engineDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("failed to create command buffers.");
    }

    for(int i = 0; i < commandBuffers.size(); ++i){
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS){
            throw std::runtime_error(std::format("CommandBuffer {} failed to begin recording.", i));
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = engineSwapChain.getRenderPass();
        renderPassInfo.framebuffer = engineSwapChain.getFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = engineSwapChain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        enginePipeline->bind(commandBuffers[i]);
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);
        if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
            throw std::runtime_error(std::format("failed to record commandBuffer ", i));
        }
    }
}

void Cnake::drawFrame(){
    uint32_t imageIndex;
    VkResult result = engineSwapChain.acquireNextImage(&imageIndex);

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        throw std::runtime_error("failed to acquire the next swapChain image.");
    }

    result = engineSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if(result != VK_SUCCESS){
        throw std::runtime_error(std::format("failed to present swapChain image at index {}", imageIndex));
    }
}

} //namespace engine
