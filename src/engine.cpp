#include "h/engine.h"
#include "vulkan/vulkan_core.h"
#include <algorithm>
#include <map>

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

void Engine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
    createInfo = {}; 
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; 
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void Engine::setupDebugMessenger(){
    if(!enableValidationLayers){ return; }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)){
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

std::vector<const char*> Engine::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(enableValidationLayers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool Engine::checkValidationLayerSupport(){
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

bool Engine::checkExtensionSupport(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt){
    std::cout << "\n\tInstance presents:\n";
    for(const auto &extension : *instanceExt){
        std::cout << "\t" << extension.extensionName << '\n';
    }

    std::cout << "\n\tRequired:\n"; 
    for(const auto &required : *requiredExt){
        bool extFound = false;
        
            for(const auto &present : *instanceExt){
                if(0 == strcmp(required, present.extensionName)){
                    std::cout << "found:\t" << required << '\n';
                    extFound = true;
                    break;
                }
            }
        if(!extFound){ 
            std::cout << "not found: \t" << required << '\n';
            return false; 
        } 
    }

    return true;
}

uint32_t Engine::rateDeviceSuitability(VkPhysicalDevice device){
    uint32_t score = 0;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if(!deviceFeatures.geometryShader){ return 0; }

    QueueFamilyIndices indices = findQueueFamilies(device);
    if(!indices.isComplete()){ return 0; }
    
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
        score += 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

void Engine::pickPhysicalDevice(){
    uint32_t deviceCount = 0;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    
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

QueueFamilyIndices Engine::findQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for(const auto &queueFamily : queueFamilies){
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        if(indices.isComplete()){ break; }

        ++i;
    }

    return indices;
}

void Engine::initVulkan(){
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
}

void Engine::cleanup(){
    if(enableValidationLayers){
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}
