#include "engine.h"

#include <algorithm>

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats){
    for(const auto &availableFormat : availableFormats){
        if(VK_FORMAT_B8G8R8_SRGB == availableFormat.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == availableFormat.colorSpace){
            return availableFormat;
        }
    }
    
    return availableFormats[0];
}

VkPresentModeKHR engine::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes){
    for(const auto &availablePresentMode : availablePresentModes){
        if(VK_PRESENT_MODE_IMMEDIATE_KHR == availablePresentMode){
            printDebugLog("present mode: VK_PRESENT_MODE_IMMEDIATE_KHR", 0, 2);
            return availablePresentMode;
        }
    }

    for(const auto &availablePresentMode : availablePresentModes){
        if(VK_PRESENT_MODE_MAILBOX_KHR == availablePresentMode){
            printDebugLog("present mode: VK_PRESENT_MODE_MAILBOX_KHR", 0, 2);
            return availablePresentMode;
        }
    }

    printDebugLog("present mode: VK_PRESENT_MODE_FIFO_KHR", 0, 2);
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D engine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities){
    if(std::numeric_limits<uint32_t>::max() != capabilities.currentExtent.width){
        printDebugLog("swap width: ", 0, 0);
        printDebugLog(std::to_string(capabilities.currentExtent.width), 0, 1);
        printDebugLog("swap height: ", 0, 0);
        printDebugLog(std::to_string(capabilities.currentExtent.height), 0, 2);

        return capabilities.currentExtent; 
    }else{
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        printDebugLog("swap width: ", 0, 0);
        printDebugLog(std::to_string(actualExtent.width), 0, 1);
        printDebugLog("swap height: ", 0, 0);
        printDebugLog(std::to_string(actualExtent.height), 0, 2);
        
        return actualExtent;
    }
}

void engine::createSwapChain(){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if(0 < swapChainSupport.capabilities.maxImageCount && imageCount > swapChainSupport.capabilities.maxImageCount){
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent; 
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if(indices.graphicsFamily != indices.presentFamily){
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }else{
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create swap chain!", 2, 1);
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}
