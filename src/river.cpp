#include "river.h"
#include "engine.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

void river::initVulkan(){

    engine::createInstance();
    engine::setupDebugMessenger();
    engine::createSurface();
    device::pickPhysicalDevice();
    device::createLogicalDevice();
    swap::createSwapChain();
    swap::createImageViews();
    swap::createRenderPass();
    pipeline::createGraphicsPipeline();
    engine::createFramebuffers();
    engine::createCommandPool();
    engine::createCommandBuffer();
    engine::createSyncObjects();
}

void river::cleanupVulkan(){
    using namespace device;

    vkDeviceWaitIdle(logicalDevice);

    vkDestroySemaphore(logicalDevice, pipeline::imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(logicalDevice, pipeline::renderFinishedSemaphore, nullptr);
    vkDestroyFence(logicalDevice, pipeline::inFlightFence, nullptr);

    vkDestroyCommandPool(logicalDevice, pipeline::commandPool, nullptr);

    for(const auto &framebuffer : swap::swapChainFramebuffers){
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(logicalDevice, pipeline::graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipeline::pipelineLayout, nullptr);
    vkDestroyRenderPass(logicalDevice, pipeline::renderPass, nullptr);

    for(const auto &imageView : swap::swapChainImageViews){
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(logicalDevice, swap::swapChain, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);

    if(build_DEBUG){
        DestroyDebugUtilsMessengerEXT(engine::instance, debugMessenger, nullptr); 
    }

    vkDestroySurfaceKHR(engine::instance, swap::surface, nullptr);
    vkDestroyInstance(engine::instance, nullptr);
}

void river::drawFrame(){
    using namespace pipeline;

    uint32_t imageIndex;

    vkWaitForFences(device::logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device::logicalDevice, 1, &inFlightFence);

    vkAcquireNextImageKHR(device::logicalDevice, swap::swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    
    vkResetCommandBuffer(commandBuffer, 0);
    engine::recordCommandBuffer(commandBuffer, imageIndex);

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.signalSemaphoreCount = 1;

    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &pipeline::commandBuffer;

    if(vkQueueSubmit(engine::graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS){
        engine::printDebugLog("\nERROR: failed to submit draw command buffer!", 2, 1);
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChains[] = {swap::swapChain};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(engine::presentQueue, &presentInfo);
}
