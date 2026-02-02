#include "river.h"
#include "engine.h"
#include "device.h"

void river::initVulkan(){
    using namespace engine;

    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

void river::cleanupVulkan(){
    using namespace engine;

    vkDeviceWaitIdle(device::logicalDevice);

    vkDestroySemaphore(device::logicalDevice, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device::logicalDevice, renderFinishedSemaphore, nullptr);
    vkDestroyFence(device::logicalDevice, inFlightFence, nullptr);

    vkDestroyCommandPool(device::logicalDevice, commandPool, nullptr);

    for(const auto &framebuffer : swapChainFramebuffers){
        vkDestroyFramebuffer(device::logicalDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(device::logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device::logicalDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(device::logicalDevice, renderPass, nullptr);

    for(const auto &imageView : swapChainImageViews){
        vkDestroyImageView(device::logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device::logicalDevice, swapChain, nullptr);
    vkDestroyDevice(device::logicalDevice, nullptr);

    if(build_DEBUG){
        device::DestroyDebugUtilsMessengerEXT(instance, device::debugMessenger, nullptr); 
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void river::drawFrame(){
    using namespace engine;

    uint32_t imageIndex;

    vkWaitForFences(device::logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device::logicalDevice, 1, &inFlightFence);

    vkAcquireNextImageKHR(device::logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, imageIndex);

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
    submitInfo.pCommandBuffers = &commandBuffer;

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to submit draw command buffer!.", 2, 1);
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChains[] = {swapChain};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);
}
