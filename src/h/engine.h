#pragma once

#define GLFW_INCLUDE_VULKAN
#include "vulkan/vulkan_core.h"
#include "GLFW/glfw3.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else 
    const bool enableValidationLayers = true;
#endif

struct Engine{

    Engine() = default;
    Engine(const Engine&) = delete;
    Engine &operator=(const Engine&) = delete;
    Engine(GLFWwindow *&window) : window(window){};

    uint32_t instanceExtensionCount = 0;
    uint32_t glfwExtensionCount = 0;

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

    VkInstance instance;
    GLFWwindow *window;

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
    uint32_t findQueueFamilies(VkPhysicalDevice device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT             messageType, 
            const VkDebugUtilsMessengerCallbackDataEXT  *pCallbackData,
            void                                        *pUserData
        ){

        std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';

        return VK_FALSE;
    }

    void createInstance();
    void initVulkan();
    void cleanup();
};
