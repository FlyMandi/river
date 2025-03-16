#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>

#ifdef NDEBUG
    const bool build_DEBUG = false;
#else 
    const bool build_DEBUG = true;
#endif

inline const char *engineName = "River";

inline const int MAX_FRAMES_IN_FLIGHT = 2;

inline VkInstance instance;
inline VkDebugUtilsMessengerEXT debugMessenger;

inline VkPhysicalDevice physicalDevice;
inline VkPhysicalDeviceProperties deviceProperties;
inline VkPhysicalDeviceFeatures deviceFeatures;

inline VkDevice device;

inline VkQueue graphicsQueue;
inline VkQueue presentQueue;

inline VkSurfaceKHR surface;
inline VkSwapchainKHR swapChain;
inline VkFormat swapChainImageFormat;
inline VkExtent2D swapChainExtent;

inline std::vector<VkImage> swapChainImages;
inline std::vector<VkImageView> swapChainImageViews;
inline std::vector<VkFramebuffer> swapChainFramebuffers;

inline VkRenderPass renderPass;
inline VkPipelineLayout pipelineLayout;
inline VkPipeline graphicsPipeline;
inline VkCommandPool commandPool;
inline VkCommandBuffer commandBuffer;

inline VkSemaphore imageAvailableSemaphore;
inline VkSemaphore renderFinishedSemaphore;
inline VkFence inFlightFence;

void initVulkan();
void cleanupVulkan();
void drawFrame();
