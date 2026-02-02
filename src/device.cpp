#include "river.h"
#include "vulkan/vulkan_core.h"
#include "window.h"
#include "device.h"
#include "pipeline.h"

#include <set>
#include <map>

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
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

bool findQueueFamilies(VkPhysicalDevice device)
{
    static uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    static std::vector<VkQueueFamilyProperties> physicalQueueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, physicalQueueFamilies.data());

    VkBool32 presentSupport = false;

    for(int i = 0; const auto &queueFamily : physicalQueueFamilies)
    {
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            logicalQueueFamilies.graphicsIndex = i;

        }else if(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT){
            logicalQueueFamilies.transferIndex = i; 
        }
        
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport){
            logicalQueueFamilies.presentIndex = i;
        }

        if( logicalQueueFamilies.graphicsIndex != -1 && 
            logicalQueueFamilies.transferIndex != -1 &&
            logicalQueueFamilies.presentIndex != -1){ 
            break; 
        }
        ++i;
    }

    if(logicalQueueFamilies.transferIndex == -1){
        logicalQueueFamilies.transferIndex = logicalQueueFamilies.graphicsIndex;
    }

    return( logicalQueueFamilies.graphicsIndex != -1 &&
            logicalQueueFamilies.transferIndex != -1 &&
            logicalQueueFamilies.presentIndex != -1);
}

static uint32_t rateDeviceSuitability(VkPhysicalDevice device)
{
    uint32_t score = 0;

    if(!findQueueFamilies(device)){ 
        return 0; 
    }

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if(!deviceFeatures.geometryShader){ 
        return 0; 
    }

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

    if(logicalQueueFamilies.presentIndex == logicalQueueFamilies.graphicsIndex){
        score += 500;
    }
    
    if(logicalQueueFamilies.transferIndex != logicalQueueFamilies.graphicsIndex){
        score += 250;
    }

    #ifdef DEBUG
        printDebugLog('\0', deviceProperties.deviceName);
        printDebugLog(", score: ");
        printDebugLog(score, '\n');
    #endif

    return score;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
{
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

void pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    physicalDevice = VK_NULL_HANDLE;
    
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if(0 == deviceCount){
        #ifdef DEBUG
            printDebugLog("failed to find any GPU with vulkan support!");
        #endif
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
        #ifdef DEBUG
            printDebugLog('\0', "found suitable GPU.", '\n');
        #endif

    }else{
        #ifdef DEBUG
            printDebugLog("failed to find a suitable GPU!");
        #endif
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void createLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        logicalQueueFamilies.graphicsIndex,
        logicalQueueFamilies.transferIndex,
        logicalQueueFamilies.presentIndex
    };

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

    #ifdef DEBUG
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    #else
        createInfo.enabledLayerCount = 0;
    #endif

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS){
        #ifdef DEBUG
            printDebugLog('\n', "failed to create logical device.");
        #endif
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(logicalDevice, logicalQueueFamilies.graphicsIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, logicalQueueFamilies.transferIndex, 0, &transferQueue);
    vkGetDeviceQueue(logicalDevice, logicalQueueFamilies.presentIndex, 0, &presentQueue);

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
}
