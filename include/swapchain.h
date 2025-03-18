#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

namespace swap{

extern VkSurfaceKHR surface;
extern VkSwapchainKHR swapChain;
extern VkFormat swapChainImageFormat;
extern VkExtent2D swapChainExtent;

extern std::vector<VkImage> swapChainImages;
extern std::vector<VkImageView> swapChainImageViews;
extern std::vector<VkFramebuffer> swapChainFramebuffers;

extern void createSwapChain();
extern void createImageViews();
extern void createRenderPass();

};
