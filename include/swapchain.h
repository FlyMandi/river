#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

constexpr uint8_t MAX_FRAMES_IN_FLIGHT = 2;

inline VkSwapchainKHR swapchain;
inline VkFormat swapchainImageFormat;
inline VkExtent2D swapchainExtent;

inline std::vector<VkImage> swapchainImages;
inline std::vector<VkImageView> swapchainImageViews;
inline std::vector<VkFramebuffer> swapchainFramebuffers;

inline uint32_t swapchainImageCount;

//HACK: only one of each right now, eventually all of those will be arrays
struct FrameResource
{
    VkFramebuffer framebuffer2Destroy;
    VkBool32 hasFramebuffer;

    VkImageView imageView2Destroy;
    VkBool32 hasImageView;
};

inline FrameResource frameResources[MAX_FRAMES_IN_FLIGHT];

extern void createSwapchain();
extern void createImageViews();
extern void createRenderPass();

extern void destroyDeferredResources(FrameResource *frame);
extern void cleanupSwapchain();
extern void recreateSwapchain();

extern VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
