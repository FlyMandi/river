#pragma once

#include "vulkan/vulkan_core.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <optional>

#ifdef NDEBUG
    const bool build_DEBUG = false;
#else 
    const bool build_DEBUG = true;
#endif

inline const char *engineName = "MagmaRiver";

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

void createInstance();
void setupDebugMessenger();
void createSurface();
void pickPhysicalDevice();
void createLogicalDevice();
void createSwapChain();
void createImageViews();
void createRenderPass();
void createGraphicsPipeline();
void createFramebuffers();
void createCommandPool();
void createCommandBuffer();
void createSyncObjects();

void drawFrame();

std::vector<const char*> getRequiredExtensions();
bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt);
bool checkValidationLayerSupport();

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

bool checkDeviceExtensionSupport(VkPhysicalDevice device);
uint32_t rateDeviceSuitability(VkPhysicalDevice device);

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete(){ 
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation" 
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType, 
        const VkDebugUtilsMessengerCallbackDataEXT  *pCallbackData,
        void                                        *pUserData
    ){

    std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
        VkInstance                                  instance,
        const VkDebugUtilsMessengerCreateInfoEXT    *pCreateInfo,
        const VkAllocationCallbacks                 *pAllocator,
        VkDebugUtilsMessengerEXT                    *pDebugMessenger
    );

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

void DestroyDebugUtilsMessengerEXT(
        VkInstance                  instance,
        VkDebugUtilsMessengerEXT    messenger, 
        const VkAllocationCallbacks *pAllocator
);
