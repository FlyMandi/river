#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

class CnakeApp{
    const char *version = "Cnake 0.0.0";
    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

    GLFWwindow *window;
    VkInstance instance;

    void initWindow(){
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, version, nullptr, nullptr);
    }

    void verifyExtensionPresence(const char **glfwExt, std::vector<VkExtensionProperties> vulkanExt){
        std::string vulkanExtString = "";
        std::cout << "GLFW requires:\n" << '\t' << *glfwExt << '\n';
        std::cout << "\nVulkan presents:\n";

        for(const auto &extension : vulkanExt){
            vulkanExtString += extension.extensionName;
            std::cout << '\t' << extension.extensionName << '\n';
        }

        if(vulkanExtString.find(*glfwExt) == std::string::npos){
            throw std::runtime_error("not all needed extensions present.");
        }
        std::cout << "\nAll needed extensions are present.";
    }

    void createInstance(){
        uint32_t extensionCount = 0;
        uint32_t glfwExtensionCount = 0;

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount); 
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        verifyExtensionPresence(glfwExtensions, extensions);

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Cnake";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if(vkCreateInstance(&createInfo, nullptr, &instance)){
            throw std::runtime_error("failed to create instance.");
        }
    }

    void initVulkan(){
        createInstance();
    }

    void gameLoop(){
        while(!glfwWindowShouldClose(window)){
           glfwPollEvents(); 
        }
    }

    void cleanup(){
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

 public:
    void run(){
        initWindow();
        initVulkan();
        gameLoop();
        cleanup();
    }
};

int main(){
    CnakeApp app;

    try{
        app.run();
    }catch(const std::exception &e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
