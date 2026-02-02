#pragma once

#include "vulkan/vulkan_core.h"

#include <cstdint>
#include <vector>

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    uint32_t graphicsIndex = UINT32_MAX;
    uint32_t transferIndex = UINT32_MAX;
    uint32_t presentIndex  = UINT32_MAX;
};

inline QueueFamilyIndices logicalQueueFamilies;

inline VkPhysicalDevice physicalDevice;
inline VkPhysicalDeviceProperties deviceProperties;
inline VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
inline VkPhysicalDeviceFeatures deviceFeatures;

inline VkDevice logicalDevice;

extern SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
extern QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

extern void pickPhysicalDevice();
extern void createLogicalDevice();
