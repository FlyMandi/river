#pragma once

#define GLFW_INCLUDE_VULKAN
#include "vulkan/vulkan_core.h"
#include "GLFW/glfw3.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>

class Engine{

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else 
    const bool enableValidationLayers = true;
#endif

    //TODO: delete copy operator & constructor
    //deal with constructors & destructors, default constructors

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
 public:
    void createInstance();
    void initVulkan();
    void cleanup();
};
