#pragma once

#include "window.h"

#include "vulkan/vulkan_core.h"

#include <optional>
#include <vector>

typedef struct SwapChainSupportDetails{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
}SwapChainSupportDetails;

typedef struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete(){ 
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
}QueueFamilyIndices;

typedef struct Device{

Device() = delete;
Device(Window &window) : window(window){};
Device(const Device&) = delete;
Device operator=(const Device &) = delete;
~Device();

Window &window;

VkPhysicalDevice physicalDevice;
VkPhysicalDeviceProperties deviceProperties;
VkPhysicalDeviceFeatures deviceFeatures;

VkDevice logicalDevice;

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

static uint32_t rateDeviceSuitability(VkPhysicalDevice device);

void pickPhysicalDevice();
void createLogicalDevice();

}Device;
