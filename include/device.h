#pragma once

#include "vulkan/vulkan_core.h"

#include <optional>
#include <vector>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete(){ 
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

inline VkPhysicalDevice physicalDevice;
inline VkPhysicalDeviceProperties deviceProperties;
inline VkPhysicalDeviceFeatures deviceFeatures;
inline VkDevice logicalDevice;

extern SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
extern QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

extern void pickPhysicalDevice();
extern void createLogicalDevice();
