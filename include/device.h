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
    std::optional<uint32_t> transferFamily;

    bool isComplete(){ 
        return  graphicsFamily.has_value() && 
                presentFamily.has_value() &&
                transferFamily.has_value();
    }
};

inline VkPhysicalDevice physicalDevice;
inline VkPhysicalDeviceProperties deviceProperties;
inline VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
inline VkPhysicalDeviceFeatures deviceFeatures;
inline VkDevice logicalDevice;


extern SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
extern QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

extern void pickPhysicalDevice();
extern void createLogicalDevice();
