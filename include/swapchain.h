#pragma once

#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include <vector>

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

inline VkSwapchainKHR swapchain;
inline VkFormat swapchainImageFormat;
inline VkExtent2D swapchainExtent;

inline std::vector<VkImage> swapchainImages{};
inline std::vector<VkImageView> swapchainImageViews{};
inline std::vector<VkFramebuffer> swapchainFramebuffers{};

inline uint32_t swapchainImageCount;

extern void createSwapchain();
extern void createSwapImageViews();
extern void createRenderPass();

extern void cleanupSwapchain();
extern void recreateSwapchain();

extern VkExtent2D chooseSwapExtent
(
    const VkSurfaceCapabilitiesKHR  &capabilities,
    GLFWwindow                      *window
);
