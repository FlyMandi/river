#include "river.h"
#include "window.h"
#include "device.h"
#include "pipeline.h"

#include <set>
#include <map>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static bool checkDeviceExtensionSupport(VkPhysicalDevice device){
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

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device){
    static QueueFamilyIndices indices;
    static uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    static std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    VkBool32 presentSupport = false;

    for(int i = 0; const auto &queueFamily : queueFamilies){
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport){
            indices.presentFamily = i;
        }

        if(indices.isComplete()){ 
            break; 
        }
        ++i;
    }

    return indices;
}

static uint32_t rateDeviceSuitability(VkPhysicalDevice device){
    uint32_t score = 0;

    QueueFamilyIndices indices = findQueueFamilies(device);
    if(!indices.isComplete()){ 
        return 0; 
    }

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

    printDebugLog(deviceProperties.deviceName, 0, 1);
    printDebugLog("score: ", 0, 0);
    printDebugLog(std::to_string(score), 0, 2);

    return score;
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

void pickPhysicalDevice(){
    uint32_t deviceCount = 0;
    physicalDevice = VK_NULL_HANDLE;
    
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if(0 == deviceCount){
        printDebugLog("\nERROR: failed to find any GPU with vulkan support!", 2, 1);
        throw std::runtime_error("failed to find any GPU with vulkan support!");
    }else{
        printDebugLog("found GPU with vulkan support!", 0, 1);
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
        printDebugLog("found suitable GPU.", 0, 1);
    }else{
        printDebugLog("\nERROR: failed to find a suitable GPU!", 2, 1);
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void createLogicalDevice()
{
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

    if(BUILD_DEBUG){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }else{
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create logical device.", 2, 1);
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}
