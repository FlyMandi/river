#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct queueFamilyIndices
{
    uint32_t graphicsIndex = -1;
    uint32_t transferIndex = -1;
    uint32_t presentIndex = -1;
};

inline queueFamilyIndices logicalQueueFamilies;

inline VkPhysicalDevice physicalDevice;
inline VkPhysicalDeviceProperties deviceProperties;
inline VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
inline VkPhysicalDeviceFeatures deviceFeatures;

inline VkDevice logicalDevice;

extern SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
extern bool findQueueFamilies(VkPhysicalDevice device);

extern void pickPhysicalDevice();
extern void createLogicalDevice();
