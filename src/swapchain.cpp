#include "h/engine.h"
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
