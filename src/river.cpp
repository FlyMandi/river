#include "vulkan/vulkan_core.h"
#include "river.h"
#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffer.h"
#include "image.h"

#include <cstdint>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iostream>

static const char* logLevelStamps[] =
{
    "[RIV_TRACE]: ",
    "[RIV_DEBUG]: ",
    "[RIV_WARNI]: ",
    "[RIV_ERROR]: ",
    "[RIV_ASSER]: "
};

static const char* logLevelANSI[] =
{
    "\033[30;1;1m",
    "\033[37;1;1m",
    "\033[33;1;1m",
    "\033[31;1;1m",
    "\033[31;1;7m"
};

static const char* clearANSI = "\033[0m";

#ifdef DEBUG

RiverLogLevel severityTranslation(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
{
    switch(severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:   return RIV_LOG_LEVEL_TRACE;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:      return RIV_LOG_LEVEL_DEBUG;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:   return RIV_LOG_LEVEL_WARN;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:     return RIV_LOG_LEVEL_ERROR;
        default:                                                return RIV_LOG_LEVEL_UNDEFINED;
    }
}

const char* riverTranslateVkResult(VkResult code)
{
    switch(code)
    {
        case VK_SUCCESS:                                            return "VK_SUCCESS";
        case VK_NOT_READY:                                          return "VK_NOT_READY";
        case VK_TIMEOUT:                                            return "VK_TIMEOUT";
        case VK_EVENT_SET:                                          return "VK_EVENT_SET";
        case VK_EVENT_RESET:                                        return "VK_EVENT_RESET";
        case VK_INCOMPLETE:                                         return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:                           return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:                         return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:                        return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:                                  return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:                            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:                            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:                        return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:                          return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:                          return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:                             return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:                         return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:                              return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:                                      return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:                           return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:                      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:                                return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:               return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED:                          return "VK_PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_NOT_PERMITTED:                                return "VK_ERROR_NOT_PERMITTED";
        case VK_ERROR_SURFACE_LOST_KHR:                             return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:                     return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:                                     return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:                              return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:                     return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:                        return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:                            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:                return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:       return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:    return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:       return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:        return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:          return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:          return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR:                                    return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR:                                    return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR:                             return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR:                         return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:             return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:                    return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        case VK_INCOMPATIBLE_SHADER_BINARY_EXT:                     return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
        case VK_PIPELINE_BINARY_MISSING_KHR:                        return "VK_PIPELINE_BINARY_MISSING_KHR";
        case VK_ERROR_NOT_ENOUGH_SPACE_KHR:                         return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
        default:                                                    return "RIV_ERROR_VK_ERROR_NOT_TRANSLATED";
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT  *callbackData,
    void                                        *userData
){
    //can I know which layer is outputting the msg?
    std::string msg = "VL says: ";
    msg += callbackData->pMessage;

    riverLog(msg.c_str(), severityTranslation(messageSeverity));
    return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT
(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT    *createInfo,
    const VkAllocationCallbacks                 *allocator,
    VkDebugUtilsMessengerEXT                    *debugMessenger
){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(nullptr != func)
    {
        return func(instance, createInfo, allocator, debugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static VkBool32 checkValidationLayerSupport()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layerVec(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerVec.data());

    for(const char *layer : validationLayers)
    {
        VkBool32 layerFound = VK_FALSE;

        for(const auto &layerPresent : layerVec)
        {
            if(0 == strcmp(layerPresent.layerName, layer))
            {
                layerFound = VK_TRUE;
                break;
            }
        }
        if(!layerFound)
        {
            std::string msg = "validation layer not found: ";
            msg += layer;

            riverLog
            (
                msg.c_str(),
                RIV_LOG_LEVEL_WARN
            );
            return VK_FALSE;
        }
    }

    return VK_TRUE;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

static void setupDebugMessenger()
{

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    riverAssertVkSuccess
    (
        CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger),
        "failed to set up debug messenger."
    );
}

static void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks *allocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(nullptr != func)
    {
        func(instance, debugMessenger, allocator);
    }
}
#endif

void riverLog(const char* text, const RiverLogLevel level)
{
    if(level < logLevel)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cout << logLevelANSI[level];

    std::cout << '\n' << std::put_time(&buf, "[%T] ") << logLevelStamps[level];

    if(level == RIV_LOG_LEVEL_WARN || level == RIV_LOG_LEVEL_ERROR)
    {
        std::cerr << text << clearANSI;
        return;
    }

    std::cout << text << clearANSI;
}

void riverAssert(bool condition, const char* assertFailureMsg)
{
    if(condition)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << std::put_time(&buf, "[%T] ") << logLevelANSI[RIV_LOG_LEVEL_ASSERT]
        << logLevelStamps[RIV_LOG_LEVEL_ASSERT] << assertFailureMsg << clearANSI;

    abort();
}

void riverAssertVkSuccess(VkResult result, const char* assertFailureMsg)
{
    if(result == VK_SUCCESS)
    {
        return;
    }

    uint32_t logLevel = RIV_LOG_LEVEL_ASSERT;

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << logLevelANSI[logLevel] << std::put_time(&buf, "[%T] ") << logLevelStamps[logLevel]
        << riverTranslateVkResult(result) << ": " << assertFailureMsg << clearANSI;

    abort();
}

void riverThrow(const char *throwMsg)
{
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << logLevelANSI[RIV_LOG_LEVEL_ASSERT] << std::put_time(&buf, "[%T] ")
        << logLevelStamps[RIV_LOG_LEVEL_ASSERT] << ": " << throwMsg << clearANSI;
}

static std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    #ifdef DEBUG
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

    return extensions;
}

static VkBool32 checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt)
{
    for(const auto &required : *requiredExt)
    {
        VkBool32 extFound = VK_FALSE;

            for(const auto &present : *instanceExt)
            {
                if(0 == strcmp(required, present.extensionName))
                {
                    extFound = VK_TRUE;
                    break;
                }
            }
        if(!extFound)
        {
            std::string msg = "extension not found: ";
            msg += required;

            riverLog(msg.c_str(), RIV_LOG_LEVEL_ERROR);

            return VK_FALSE;
        }
    }
    return VK_TRUE;
}

static void createInstance()
{
    #ifdef DEBUG
        riverAssert(checkValidationLayerSupport(), "validation layers requested, but not available!");
    #endif

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = riverAppName;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    appInfo.pEngineName = ENGINE_NAME;
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data());

    std::vector<const char*> requiredExtensions = getRequiredExtensions();

    riverAssert
    (
        checkInstanceExtensions(&requiredExtensions, &instanceExtensions),
        "extensions required, but not available!"
    );

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.enabledLayerCount = 0;

    #ifdef DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    #else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    #endif

    riverAssertVkSuccess
    (
        vkCreateInstance(&createInfo, nullptr, &instance),
        "failed to create instance."
    );
}

void initVulkan()
{
    createInstance();

    #ifdef DEBUG
        setupDebugMessenger();
    #endif

    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();

    createSwapchain();
    createImageViews();

    createRenderPass();
    createDescriptorSetLayout();

    createGraphicsPipeline();
    createFramebuffers();

    createCommandPools();
    createTextureImage();

    createVertexBuffer();
    createUniformBuffers();

    createDescriptorPool();
    createDescriptorSets();

    createCommandBuffers();
    createSyncObjects();
}

void cleanupVulkan()
{
    std::vector<VkSemaphore> semaphores;
    semaphores.reserve(imageAvailableSemaphores.size() + renderFinishedSemaphores.size());
    semaphores.insert(semaphores.end(), imageAvailableSemaphores.begin(), imageAvailableSemaphores.end());
    semaphores.insert(semaphores.end(), renderFinishedSemaphores.begin(), renderFinishedSemaphores.end());

    VkSemaphoreWaitInfo semaphoreWaitInfo{};
    semaphoreWaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    semaphoreWaitInfo.semaphoreCount = static_cast<uint32_t>(semaphores.size());
    semaphoreWaitInfo.pSemaphores = semaphores.data();

    vkWaitSemaphores(logicalDevice, &semaphoreWaitInfo, UINT64_MAX);

    cleanupSwapchain();

    vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);

    vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);

    vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice, graphicsPipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);

    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);

        vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);

        vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
        vkFreeMemory(logicalDevice, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyCommandPool(logicalDevice, graphicsCommandPool, nullptr);
    vkDestroyCommandPool(logicalDevice, transferCommandPool, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);

    #ifdef DEBUG
        DestroyDebugUtilsMessengerEXT(nullptr);
    #endif

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void drawFrame()
{
    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;

    VkResult result =
        vkAcquireNextImageKHR
        (
            logicalDevice,
            swapchain,
            UINT64_MAX,
            imageAvailableSemaphores[currentFrame],
            VK_NULL_HANDLE,
            &imageIndex
        );

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain();
        return;
    }
    else
    {
        riverAssertVkSuccess(result, "failed to acquire swapchain image!");
    }

    //I'm not sure about necessary destruction, actually
    destroyDeferredResources(&frameResources[currentFrame]);
    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    updateUniformBuffer(currentFrame);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkPipelineStageFlags waitStages[] =
    {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSubmitInfo drawSubmitInfo{};
    drawSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    drawSubmitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
    drawSubmitInfo.waitSemaphoreCount = 1;
    drawSubmitInfo.pSignalSemaphores = &renderFinishedSemaphores[imageIndex];
    drawSubmitInfo.signalSemaphoreCount = 1;

    drawSubmitInfo.pWaitDstStageMask = waitStages;
    drawSubmitInfo.commandBufferCount = 1;
    drawSubmitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    riverAssertVkSuccess
    (
        vkQueueSubmit(graphicsQueue, 1, &drawSubmitInfo, inFlightFences[currentFrame]),
        "failed to submit draw command buffer!"
    );

    VkSwapchainKHR swapchains[] =
    {
        swapchain
    };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[imageIndex];

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = VK_FALSE;
        recreateSwapchain();
    }
    else
    {
        riverAssertVkSuccess(result, "failed to present swapchain image!");
    }

    currentFrame = (++currentFrame) % MAX_FRAMES_IN_FLIGHT;
}

std::filesystem::path getProjectRoot(const char *rootName)
{
    std::filesystem::path current = std::filesystem::canonical(std::filesystem::current_path());

    for(int i = 0; i < 256; ++i)
    {
        if(strcmp(current.filename().string().c_str(), rootName) == 0)
        {
            return current;
        }
        current = current.parent_path();
    }
    return current;
}

void clearLogs(const std::filesystem::path &baseDir)
{
    for(const auto &log : std::filesystem::directory_iterator(baseDir))
    {
        if(".log" == log.path().extension())
        {
            std::filesystem::remove(log);
        }
    }
}
