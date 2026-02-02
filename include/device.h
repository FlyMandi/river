#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

inline uint32_t graphicsFamilyIndex = -1;
inline uint32_t presentFamilyIndex = -1;
inline uint32_t transferFamilyIndex = -1;

inline VkPhysicalDevice physicalDevice;
inline VkPhysicalDeviceProperties deviceProperties;
inline VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
inline VkPhysicalDeviceFeatures deviceFeatures;
inline VkDevice logicalDevice;


extern SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
extern void findQueueFamilies(VkPhysicalDevice device);

extern void pickPhysicalDevice();
extern void createLogicalDevice();
