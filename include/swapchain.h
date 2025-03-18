#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

namespace swap{

inline VkSurfaceKHR surface;
inline VkSwapchainKHR swapChain;
inline VkFormat swapChainImageFormat;
inline VkExtent2D swapChainExtent;

inline std::vector<VkImage> swapChainImages;
inline std::vector<VkImageView> swapChainImageViews;
inline std::vector<VkFramebuffer> swapChainFramebuffers;

extern void createSwapChain();
extern void createImageViews();
extern void createRenderPass();

};
