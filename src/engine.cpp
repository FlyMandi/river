#include "h/engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan/vulkan_core.h"

#include <stdexcept>
#include <string_view>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <set>

const char *appName = "Cnake";
const char *engineName = "CNengine";
const char *version = "0.0.0";

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

GLFWwindow *window;

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;

VkPhysicalDevice physicalDevice;
VkPhysicalDeviceProperties deviceProperties;
VkPhysicalDeviceFeatures deviceFeatures;

VkDevice device;

VkQueue graphicsQueue;
VkQueue presentQueue;

VkSurfaceKHR surface;
VkSwapchainKHR swapChain;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

std::vector<VkImage> swapChainImages;
std::vector<VkImageView> swapChainImageViews;

bool appShouldClose(){ return glfwWindowShouldClose(window); }

void initGLFW(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, version, nullptr, nullptr);
}

void cleanupGLFW(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void initVulkan(){
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createGraphicsPipeline();
}

void cleanupVulkan(){
    for(const auto &imageView : swapChainImageViews){
        vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);

    if(config_DEBUG){ DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr); }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void createInstance(){
    if(config_DEBUG && !checkValidationLayerSupport()){
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount); 
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data());

    auto requiredExtensions = getRequiredExtensions();
    if(!checkInstanceExtensions(&requiredExtensions, &instanceExtensions)){
        throw std::runtime_error("extensions required, but not available!"); 
    }

    if(config_DEBUG){ std::cout << "\nAll needed extensions are present.\n\n"; }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.enabledLayerCount = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(config_DEBUG){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); 
        createInfo.ppEnabledLayerNames = validationLayers.data();
        
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }else{
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance)){
        throw std::runtime_error("failed to create instance.");
    }
}

void setupDebugMessenger(){
    if(!config_DEBUG){ return; }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)){
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult CreateDebugUtilsMessengerEXT(
        VkInstance                                  instance,
        const VkDebugUtilsMessengerCreateInfoEXT    *pCreateInfo,
        const VkAllocationCallbacks                 *pAllocator,
        VkDebugUtilsMessengerEXT                    *pDebugMessenger
    ){

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(nullptr != func){
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }else{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
    createInfo = {}; 
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; 
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void DestroyDebugUtilsMessengerEXT(
        VkInstance                  instance,
        VkDebugUtilsMessengerEXT    messenger, 
        const VkAllocationCallbacks *pAllocator
    ){

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(nullptr != func){
        func(instance, messenger, pAllocator);
    }
}

bool checkValidationLayerSupport(){
    uint32_t layerCount = 0;
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

std::vector<const char*> getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(config_DEBUG){
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt){
    if(config_DEBUG){
        std::cout << "\n\tPresent:\n";
        for(const auto &extension : *instanceExt){
            std::cout << "\t" << extension.extensionName << '\n';
        }
    }

    if(config_DEBUG) { std::cout << "\n\tRequired:\n"; }
    for(const auto &required : *requiredExt){
        bool extFound = false;
        
            for(const auto &present : *instanceExt){
                if(0 == strcmp(required, present.extensionName)){
                    if(config_DEBUG){ std::cout << "found:\t" << required << '\n'; }
                    extFound = true;
                    break;
                }
            }
        if(!extFound){ 
            if(config_DEBUG){ std::cout << "not found: \t" << required << '\n'; }
            return false; 
        } 
    }

    return true;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device){
    uint32_t extensionCount;

    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for(const auto &extension : availableExtensions){
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device){
    SwapChainSupportDetails details;
    uint32_t formatCount;
    uint32_t presentModeCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if(0 != formatCount){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    if(0 != presentModeCount){
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

uint32_t rateDeviceSuitability(VkPhysicalDevice device){
    uint32_t score = 0;

    QueueFamilyIndices indices = findQueueFamilies(device);
    if(!indices.isComplete()){ return 0; }

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if(!deviceFeatures.geometryShader){ return 0; }

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    if(!extensionsSupported){
        return 0;
    }else{
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()){
            return 0;
        }
    }
    
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
        score += 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    if(indices.presentFamily.value() == indices.graphicsFamily.value()){
        score += 100;
    }

    if(config_DEBUG){
        std::cout << deviceProperties.deviceName << ": score " << score << '\n';
    }

    return score;
}

void pickPhysicalDevice(){
    uint32_t deviceCount = 0;
    physicalDevice = VK_NULL_HANDLE;
    
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if(0 == deviceCount){
        throw std::runtime_error("failed to find any GPU with vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    std::multimap<uint32_t, VkPhysicalDevice> suitabilityCandidates;

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for(const auto& device : devices){
        uint32_t score = rateDeviceSuitability(device);
        suitabilityCandidates.insert(std::make_pair(score, device));
    }

    if(suitabilityCandidates.rbegin()->first > 0){
        physicalDevice = suitabilityCandidates.rbegin()->second; 
    }else{
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    VkBool32 presentSupport = false;

    int i = 0;
    for(const auto &queueFamily : queueFamilies){
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport){
            indices.presentFamily = i;
        }

        if(indices.isComplete()){ break; }
        ++i;
    }

    return indices;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats){
    for(const auto &availableFormat : availableFormats){
        if(VK_FORMAT_B8G8R8_SRGB == availableFormat.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == availableFormat.colorSpace){
            return availableFormat;
        }
    }
    
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes){
    for(const auto &availablePresentMode : availablePresentModes){
        if(VK_PRESENT_MODE_IMMEDIATE_KHR == availablePresentMode){
            if(config_DEBUG){ std::cout << "present mode: VK_PRESENT_MODE_IMMEDIATE_KHR" << '\n'; }
            return availablePresentMode;
        }
    }

    for(const auto &availablePresentMode : availablePresentModes){
        if(VK_PRESENT_MODE_MAILBOX_KHR == availablePresentMode){
            if(config_DEBUG){ std::cout << "present mode: VK_PRESENT_MODE_MAILBOX_KHR" << '\n';}
            return availablePresentMode;
        }
    }

    if(config_DEBUG){ std::cout << "present mode: VK_PRESENT_MODE_FIFO_KHR" << '\n';}
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities){
    if(std::numeric_limits<uint32_t>::max() != capabilities.currentExtent.width){
        if(config_DEBUG){
            std::cout << "swap width: " << capabilities.currentExtent.width << '\n';
            std::cout << "swap height: " << capabilities.currentExtent.height << '\n';
        }
        return capabilities.currentExtent; 
    }else{
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        if(config_DEBUG){
            std::cout << "swap width: " << actualExtent.width << '\n';
            std::cout << "swap height: " << actualExtent.height << '\n';
        }

        return actualExtent;
    }
}

void createLogicalDevice(){
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for(uint32_t queueFamily : uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if(config_DEBUG){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }else{
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)){
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void createSurface(){
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("failed to create window surface!");
    }
}

void createSwapChain(){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if(0 < swapChainSupport.capabilities.maxImageCount && imageCount > swapChainSupport.capabilities.maxImageCount){
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent; 
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if(indices.graphicsFamily != indices.presentFamily){
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }else{
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS){
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void createImageViews(){
    swapChainImageViews.resize(swapChainImages.size()); 

    for(size_t i = 0; i < swapChainImages.size(); ++i){
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create image views!");
        }
    }
}

static std::vector<char> readFile(const std::string &filename){
    if(config_DEBUG){ std::cout << "reading file: " << filename << '\n'; }
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open()){
        throw std::runtime_error("failed to open file");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    if(config_DEBUG){
        std::cout << "buffer size: " << buffer.size() << '\n';
        std::cout << "file size: " << fileSize << '\n';
    }

    if(buffer.size() != fileSize){
        throw std::runtime_error("failed to correctly read from file");
    }

    file.close();
    return buffer;
}

VkShaderModule createShaderModule(const std::vector<char> &code){
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    
    if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS){
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void createGraphicsPipeline(){
    if(config_DEBUG){ std::cout << "current directory: " << std::filesystem::current_path() << '\n'; }

    auto vertShaderCode = readFile("..\\..\\src\\shaders\\vertTest.vert");
    auto fragShaderCode = readFile("..\\..\\src\\shaders\\fragTest.frag");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    //code

    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
}
