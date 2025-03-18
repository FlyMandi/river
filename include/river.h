#pragma once

#define GLFW_INCLUDE_VULKAN
#include "vulkan/vulkan_core.h"
#include "GLFW/glfw3.h"

#include <filesystem>
#include <vector>

#if defined(DEBUG) || defined(_DEBUG)
    const bool build_DEBUG = true;
#else
    const bool build_DEBUG = false;
#endif

inline bool window_SHOULDCLOSE = false;

inline const char *engineName = "River";
inline const char *appName;
inline const char *appVersion;

inline const uint32_t WIDTH = 1920;
inline const uint32_t HEIGHT = 1080;

inline GLFWwindow *window;

inline std::filesystem::path appRoot;
inline std::filesystem::path debugLog;

void initVulkan();
void cleanupVulkan();

void initGLFW();
void cleanupGLFW();

void drawFrame();
void windowPollEvents();

void getProjectRoot(const char* rootName);
void printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines);

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
