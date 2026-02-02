#include "h/cnake.h"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

bool CnakeApp::checkValidationLayerSupport(){
    uint32_t layerCount;
    
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerVec(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerVec.data());

    for(const char *layer : validationLayers){
        bool layerFound = false;

        for(const auto &layerPresent : layerVec){
            if(0 == strcmp(layerPresent.layerName, layer)){
                layerFound = true;
                break;
            }
        }
        if(!layerFound){ return false; }
    }

    return true;
}

bool CnakeApp::checkExtensionSupport(const char **glfwExt, const std::vector<VkExtensionProperties> *vulkanExt){
    std::cout << "GLFW requires:\n" << '\t' << *glfwExt << '\n';

    std::cout << "\nVulkan presents:\n";
    for(const auto &extension : *vulkanExt){
        std::cout << '\t' << extension.extensionName << '\n';
    }

    bool extFound = false;
    for(const auto &extension : *vulkanExt){
        if(0 == strcmp(extension.extensionName, *glfwExt)){
            extFound = true;
            break;
        }
    }
    if(!extFound){ return false; } 

    return true;
}

void CnakeApp::initWindow(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, version, nullptr, nullptr);
}

void CnakeApp::createInstance(){
    if(enableValidationLayers && !checkValidationLayerSupport()){
        throw std::runtime_error("validation layers requested, but not available!");
    }

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount); 
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if(!checkExtensionSupport(glfwExtensions, &extensions)){
        throw std::runtime_error("extensions required, but not available!"); 
    }

    std::cout << "\nAll needed extensions are present.";

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

    if(enableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); 
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }else{
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance)){
        throw std::runtime_error("failed to create instance.");
    }
}

void CnakeApp::initVulkan(){
    createInstance();
}

void CnakeApp::gameLoop(){
    while(!glfwWindowShouldClose(window)){
       glfwPollEvents(); 
    }
}

void CnakeApp::cleanup(){
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void CnakeApp::run(){
    initWindow();
    initVulkan();
    gameLoop();
    cleanup();
}
