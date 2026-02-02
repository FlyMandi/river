#include "river.h"
#include "vulkan/vulkan_core.h"
#include "device.h"

#include <set>
#include <map>

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

internal VkBool32 checkDeviceExtensionSupport(VkPhysicalDevice device)
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

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
    QueueFamilyIndices indices{};
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> physicalQueueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, physicalQueueFamilies.data());

    VkBool32 presentSupport = false;

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

internal uint32_t rateDeviceSuitability
(
    const EngineData        &engine,
    const VkPhysicalDevice  &device
){
    uint32_t score = 0;
    QueueFamilyIndices indices = findQueueFamilies(device, engine.surface);

    if( indices.graphicsIndex == UINT32_MAX ||
        indices.transferIndex == UINT32_MAX ||
        indices.presentIndex == UINT32_MAX
    ){
        return 0;
    }

    VkPhysicalDeviceProperties toRateDeviceProperties{};
    vkGetPhysicalDeviceProperties(device, &toRateDeviceProperties);

    VkPhysicalDeviceFeatures toRateDeviceFeatures{};
    vkGetPhysicalDeviceFeatures(device, &toRateDeviceFeatures);

    if( !toRateDeviceFeatures.samplerAnisotropy ||
        !toRateDeviceFeatures.geometryShader)
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
        SwapchainSupportDetails swapChainSupport = querySwapchainSupport(device, engine.surface);
        if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        {
            return 0;
        }
    }

    if(toRateDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }
    score += toRateDeviceProperties.limits.maxImageDimension1D;
    score += toRateDeviceProperties.limits.maxImageDimension2D/10;
    score += toRateDeviceProperties.limits.maxImageDimension3D/100;

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

SwapchainSupportDetails querySwapchainSupport
(
    const VkPhysicalDevice  &physicalDevice,
    const VkSurfaceKHR      &surface
){
    SwapchainSupportDetails details;
    uint32_t formatCount;
    uint32_t presentModeCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if(0 != formatCount)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR
        (
            physicalDevice,
            surface,
            &formatCount,
            details.formats.data()
        );
    }

    if(0 != presentModeCount)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR
        (
            physicalDevice,
            surface,
            &presentModeCount,
            details.presentModes.data()
        );
    }

    return details;
}

void pickPhysicalDevice
(
    EngineData &engine
){
    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(engine.instance, &deviceCount, nullptr);
    if(0 == deviceCount){
        riverLog("failed to find any GPU with vulkan support!", RIV_LOG_LEVEL_ERROR);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    std::multimap<uint32_t, VkPhysicalDevice> suitabilityCandidates;

    vkEnumeratePhysicalDevices(engine.instance, &deviceCount, devices.data());

    for(const auto& device : devices)
    {
        uint32_t score = rateDeviceSuitability(engine, device);
        suitabilityCandidates.insert(std::make_pair(score, device));
    }

    if(suitabilityCandidates.rbegin()->first > 0)
    {
        engine.physicalDevice = suitabilityCandidates.rbegin()->second;

        QueueFamilyIndices indices = findQueueFamilies(engine.physicalDevice, engine.surface);
        engine.logicalQueueFamilies.graphicsIndex = indices.graphicsIndex;
        engine.logicalQueueFamilies.transferIndex = indices.transferIndex;
        engine.logicalQueueFamilies.presentIndex = indices.presentIndex;
    }
    else
    {
        riverLog("failed to find a suitable GPU!", RIV_LOG_LEVEL_ASSERT);
    }
}

void createLogicalDevice
(
    EngineData &engine
){
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies =
    {
        engine.logicalQueueFamilies.graphicsIndex,
        engine.logicalQueueFamilies.transferIndex,
        engine.logicalQueueFamilies.presentIndex
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
    deviceFeatures.samplerAnisotropy = VK_TRUE;

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
        vkCreateDevice(engine.physicalDevice, &createInfo, nullptr, &engine.logicalDevice),
        "failed to create logical device."
    );

    vkGetDeviceQueue(engine.logicalDevice, engine.logicalQueueFamilies.graphicsIndex, 0, &engine.graphicsQueue);
    vkGetDeviceQueue(engine.logicalDevice, engine.logicalQueueFamilies.transferIndex, 0, &engine.transferQueue);
    vkGetDeviceQueue(engine.logicalDevice, engine.logicalQueueFamilies.presentIndex, 0, &engine.presentQueue);

    vkGetPhysicalDeviceProperties(engine.physicalDevice, &engine.deviceProperties);
    vkGetPhysicalDeviceFeatures(engine.physicalDevice, &deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(engine.physicalDevice, &engine.deviceMemoryProperties);
}
