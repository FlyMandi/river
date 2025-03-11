#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan/vulkan_core.h"

#include <vector>

class CnakeApp{
    const char *version = "Cnake 0.0.0";
    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else 
    const bool enableValidationLayers = true;
#endif

    GLFWwindow *window;
    VkInstance instance;

    bool checkValidationLayerSupport();

    void verifyExtensionPresence(const char** glfwExt, std::vector<VkExtensionProperties> vulkanExt);
    void initWindow();
    void createInstance();
    void initVulkan();
    void gameLoop();
    void cleanup();

 public:
    void run();
};
