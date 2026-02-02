#include "h/cnake.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

void CnakeApp::initWindow(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, version, nullptr, nullptr);
}

void CnakeApp::verifyExtensionPresence(const char **glfwExt, std::vector<VkExtensionProperties> vulkanExt){
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

void CnakeApp::createInstance(){
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
