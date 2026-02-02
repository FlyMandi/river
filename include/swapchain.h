#pragma once

#include "device.h"

#include "vulkan/vulkan_core.h"

#include <vector>

typedef struct SwapChain{

SwapChain() = delete;
SwapChain(Device &device) : device(device){};
SwapChain(const SwapChain&) = delete;
SwapChain operator=(const SwapChain&) = delete;
~SwapChain();

Device &device;
VkSwapchainKHR swapChain;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

std::vector<VkImage> swapChainImages;
std::vector<VkImageView> swapChainImageViews;
std::vector<VkFramebuffer> swapChainFramebuffers;

void createSwapChain();
void createImageViews();
void createRenderPass();

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

}SwapChain;
