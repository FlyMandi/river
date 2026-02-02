#pragma once

#include "vulkan/vulkan_core.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <vector>
#include <optional>

#ifdef NDEBUG
    const bool config_DEBUG = false;
#else 
    const bool config_DEBUG = true;
#endif


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

std::filesystem::path getProjectRoot();

struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete(){ 
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

bool appShouldClose();

void initVulkan();
void cleanupVulkan();
void cleanupGLFW();

void createInstance();
void createLogicalDevice();
void createSwapChain();
void createImageViews();
void createGraphicsPipeline();

std::vector<const char*> getRequiredExtensions();
bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt);
bool checkValidationLayerSupport();

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
void setupDebugMessenger();

bool checkDeviceExtensionSupport(VkPhysicalDevice device);
uint32_t rateDeviceSuitability(VkPhysicalDevice device);
void pickPhysicalDevice();      

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

void createSurface();
