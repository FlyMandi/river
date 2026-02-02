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

static VkBool32 checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;

    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for(const auto &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
{
    static QueueFamilyIndices indices{};
    static uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    static std::vector<VkQueueFamilyProperties> physicalQueueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, physicalQueueFamilies.data());

    static VkBool32 presentSupport = false;

    for(int i = 0; const auto &queueFamily : physicalQueueFamilies)
    {
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsIndex = i;
        }

        if(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && i != indices.graphicsIndex)
        {
            indices.transferIndex = i;
        }

        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport)
        {
            indices.presentIndex = i;
        }

        if( indices.graphicsIndex != UINT32_MAX &&
            indices.transferIndex != UINT32_MAX &&
            indices.presentIndex != UINT32_MAX
        ){
            break;
        }
        ++i;
    }

    if(indices.transferIndex == UINT32_MAX)
    {
        indices.transferIndex = indices.graphicsIndex;
    }

    return indices;
}

static uint32_t rateDeviceSuitability(VkPhysicalDevice device)
{
    static uint32_t score = 0;

    static QueueFamilyIndices indices = findQueueFamilies(device);
    if( indices.graphicsIndex == UINT32_MAX ||
        indices.transferIndex == UINT32_MAX ||
        indices.presentIndex == UINT32_MAX
    ){
        return 0;
    }

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if(!deviceFeatures.geometryShader)
    {
        return 0;
    }

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    if(!extensionsSupported)
    {
        return 0;
    }
    else
    {
        SwapchainSupportDetails swapChainSupport = querySwapchainSupport(device);
        if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        {
            return 0;
        }
    }

    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }
    score += deviceProperties.limits.maxImageDimension1D;
    score += deviceProperties.limits.maxImageDimension2D/10;
    score += deviceProperties.limits.maxImageDimension3D/100;

    if(indices.presentIndex == indices.graphicsIndex)
    {
        score += 500;
    }

    if(indices.transferIndex != indices.graphicsIndex)
    {
        score += 250;
    }

    return score;
}

SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device)
{
    SwapchainSupportDetails details;
    uint32_t formatCount;
    uint32_t presentModeCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if(0 != formatCount)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    if(0 != presentModeCount)
    {
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
        riverLog("failed to find any GPU with vulkan support!", RIV_LOG_LEVEL_ERROR);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    std::multimap<uint32_t, VkPhysicalDevice> suitabilityCandidates;

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for(const auto& device : devices)
    {
        uint32_t score = rateDeviceSuitability(device);
        suitabilityCandidates.insert(std::make_pair(score, device));
    }

    if(suitabilityCandidates.rbegin()->first > 0)
    {
        physicalDevice = suitabilityCandidates.rbegin()->second;

        static QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        logicalQueueFamilies.graphicsIndex = indices.graphicsIndex;
        logicalQueueFamilies.transferIndex = indices.transferIndex;
        logicalQueueFamilies.presentIndex = indices.presentIndex;
    }
    else
    {
        riverLog("failed to find a suitable GPU!", RIV_LOG_LEVEL_ASSERT);
    }
}

void createLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies =
    {
        logicalQueueFamilies.graphicsIndex,
        logicalQueueFamilies.transferIndex,
        logicalQueueFamilies.presentIndex
    };

    float queuePriority = 1.0f;
    for(uint32_t queueFamily : uniqueQueueFamilies)
    {
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

    riverAssertVkSuccess
    (
        vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice),
        "failed to create logical device."
    );

    vkGetDeviceQueue(logicalDevice, logicalQueueFamilies.graphicsIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, logicalQueueFamilies.transferIndex, 0, &transferQueue);
    vkGetDeviceQueue(logicalDevice, logicalQueueFamilies.presentIndex, 0, &presentQueue);

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
}
