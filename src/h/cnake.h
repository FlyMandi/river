#pragma once

#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan/vulkan_core.h"

#include <vector>
#include <iostream>

class CnakeApp{
    const char *version = "Cnake 0.0.0";
    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

    uint32_t instanceExtensionCount = 0;
    uint32_t glfwExtensionCount = 0;

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else 
    const bool enableValidationLayers = true;
#endif

    GLFWwindow *window;
    VkInstance instance;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    std::vector<const char*> getRequiredExtensions();
    
    bool checkExtensionSupport(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt);
    bool checkValidationLayerSupport();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void setupDebugMessenger();

    void pickPhysicalDevice();      
    uint32_t rateDeviceSuitability(VkPhysicalDevice device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT             messageType, 
            const VkDebugUtilsMessengerCallbackDataEXT  *pCallbackData,
            void                                        *pUserData
        ){

        std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';

        return VK_FALSE;
    }

    void initWindow();
    void createInstance();
    void initVulkan();
    void gameLoop();
    void cleanup();

 public:
    void run();
};
