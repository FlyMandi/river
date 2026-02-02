#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

inline VkSwapchainKHR swapchain;
inline VkFormat swapChainImageFormat;
inline VkExtent2D swapChainExtent;

inline std::vector<VkImage> swapchainImages;
inline std::vector<VkImageView> swapchainImageViews;
inline std::vector<VkFramebuffer> swapchainFramebuffers;

constexpr size_t maxFramebuffers = 1;
constexpr size_t maxBuffers = 1;
constexpr size_t maxPipelines = 1;
constexpr size_t maxRenderPasses = 1;

struct FrameResource
{
    VkFramebuffer framebuffers2Destroy[maxFramebuffers];
    size_t framebufferCount;

    VkBuffer buffers2Destroy[maxBuffers];
    size_t bufferCount;

    VkPipeline pipelines2Destroy[maxPipelines];
    size_t pipelineCount;

    VkRenderPass renderPasses2Destroy[maxRenderPasses];
    size_t renderPassCount;
};

constexpr uint16_t MAX_FRAMES_IN_FLIGHT = 2;
inline FrameResource frameResources[MAX_FRAMES_IN_FLIGHT];

extern void createSwapchain();
extern void createImageViews();
extern void createRenderPass();

extern void cleanupSwapchain();
extern void recreateSwapchain();

extern VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
