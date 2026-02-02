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

//TODO:#57: rid of inline global scope vars
inline QueueFamilyIndices logicalQueueFamilies;

inline VkPhysicalDevice physicalDevice;
inline VkPhysicalDeviceProperties deviceProperties;
inline VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
inline VkPhysicalDeviceFeatures deviceFeatures;

inline VkDevice logicalDevice;

extern SwapchainSupportDetails querySwapchainSupport
(
    const VkPhysicalDevice  &device,
    const VkSurfaceKHR      &surface
);

extern QueueFamilyIndices findQueueFamilies
(
    const VkPhysicalDevice  &device,
    const VkSurfaceKHR      &surface
);

extern void pickPhysicalDevice
(
    const VkInstance    &instance,
    const VkSurfaceKHR  &surface
);

extern void createLogicalDevice();
