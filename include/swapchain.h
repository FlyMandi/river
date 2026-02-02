#pragma once

#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include <vector>

constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

//TODO:#57: rid of inline global scope vars
inline VkSwapchainKHR swapchain;
inline VkFormat swapchainImageFormat;
inline VkExtent2D swapchainExtent;

inline std::vector<VkImage> swapchainImages{};
inline std::vector<VkImageView> swapchainImageViews{};
inline std::vector<VkFramebuffer> swapchainFramebuffers{};

inline uint32_t swapchainImageCount;

extern VkExtent2D chooseSwapExtent
(
    const VkSurfaceCapabilitiesKHR  &capabilities,
    GLFWwindow                      *window
);

extern void createSwapchain
(
    const VkSurfaceKHR      &surface,
    GLFWwindow              *window,
    const VkPresentModeKHR  &preferredPresent
);

extern void recreateSwapchain
(
    const VkSurfaceKHR      &surface,
    GLFWwindow              *window,
    const VkPresentModeKHR  &preferredPresent
);

extern void createSwapImageViews();
extern void createRenderPass();

extern void cleanupSwapchain();
