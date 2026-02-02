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

constexpr const char* logLevelStamps[] =
{
    "[RIV_TRACE]: ",
    "[RIV_DEBUG]: ",
    "[RIV_WARNI]: ",
    "[RIV_ERROR]: ",
    "[RIV_ASSER]: "
};

constexpr const char* logLevelANSI[] =
{
    "\033[30;1;1m",
    "\033[37;1;1m",
    "\033[33;1;1m",
    "\033[31;1;1m",
    "\033[31;1;7m",
    "\033[35;1;7m"
};

constexpr const char* clearANSI = "\033[0m";

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

#ifdef DEBUG

internal VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT  *callbackData,
    void                                        *userData
){
    //can I know which layer is outputting the msg?
    riverLog
    (
        std::format("VL says: {}", callbackData->pMessage),
        severityTranslation(messageSeverity)
    );
    return VK_FALSE;
}

internal VkResult CreateDebugUtilsMessengerEXT
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

internal VkBool32 checkValidationLayerSupport()
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
                msg,
                RIV_LOG_LEVEL_WARN
            );
            return VK_FALSE;
        }
    }

    return VK_TRUE;
}

internal void populateDebugMessengerCreateInfo
(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo
){
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

internal void setupDebugMessenger
(
    const VkInstance &instance
){

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    riverAssertVkSuccess
    (
        CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger),
        "failed to set up debug messenger."
    );
}

internal void DestroyDebugUtilsMessengerEXT
(
    const VkInstance            &instance,
    const VkAllocationCallbacks *allocator
){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(nullptr != func)
    {
        func(instance, debugMessenger, allocator);
    }
}
#endif

internal std::vector<const char*> getRequiredExtensions()
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

internal VkBool32 checkInstanceExtensions
(
    std::vector<const char*>            *requiredExt,
    std::vector<VkExtensionProperties>  *instanceExt
){
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

            riverLog(msg, RIV_LOG_LEVEL_ERROR);

            return VK_FALSE;
        }
    }
    return VK_TRUE;
}

internal void createInstance
(
    EngineData              &engine,
    const ProjectManifest   &manifest
){
    #ifdef DEBUG
        riverAssert(checkValidationLayerSupport(), "validation layers requested, but not available!");
    #endif

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = manifest.projectName.c_str();
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
        vkCreateInstance(&createInfo, nullptr, &engine.instance),
        "failed to create instance."
    );
}

void initVulkan
(
    EngineData              &engine,
    const ProjectManifest   &manifest,
    const UserSettings      &settings
){
    createInstance(engine, manifest);

    #ifdef DEBUG
        setupDebugMessenger(engine.instance);
    #endif

    createSurface(engine);
    pickPhysicalDevice(engine);
    createLogicalDevice(engine);

    createSwapchain(engine, settings);

    createRenderPass(engine);
    createDescriptorSetLayout(engine);
    createGraphicsPipeline(engine, manifest);

    createCommandPools(engine);
    createDepthResources(engine);
    createFramebuffers(engine);

    createTextureImage(engine, manifest);
    createTextureSampler(engine);

    loadModel(engine, manifest.projectModelPath);
    createVertexBuffer(engine);
    createUniformBuffers(engine);

    createDescriptorPool(engine);
    createDescriptorSets(engine);

    createCommandBuffers(engine);
    createSyncObjects(engine);
}

void cleanupVulkan
(
    EngineData &engine
){
    vkDeviceWaitIdle(engine.logicalDevice);

    cleanupSyncObjects(engine);
    cleanupSwapchain(engine);

    vkDestroySampler(engine.logicalDevice, engine.textureSampler, nullptr);

    vkDestroyImage(engine.logicalDevice, engine.textureImage, nullptr);
    vkFreeMemory(engine.logicalDevice, engine.textureImageMemory, nullptr);
    vkDestroyImageView(engine.logicalDevice, engine.textureImageView, nullptr);

    vkDestroyBuffer(engine.logicalDevice, engine.vertexBuffer, nullptr);
    vkFreeMemory(engine.logicalDevice, engine.vertexBufferMemory, nullptr);

    vkDestroyDescriptorPool(engine.logicalDevice, engine.descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.logicalDevice, engine.descriptorSetLayout, nullptr);

    vkDestroyPipeline(engine.logicalDevice, engine.graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(engine.logicalDevice, engine.graphicsPipelineLayout, nullptr);

    vkDestroyRenderPass(engine.logicalDevice, engine.renderPass, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroyBuffer(engine.logicalDevice, engine.uniformBuffers[i], nullptr);
        vkFreeMemory(engine.logicalDevice, engine.uniformBuffersMemory[i], nullptr);
    }

    vkDestroyCommandPool(engine.logicalDevice, engine.graphicsCommandPool, nullptr);
    vkDestroyCommandPool(engine.logicalDevice, engine.transferCommandPool, nullptr);
    vkDestroyDevice(engine.logicalDevice, nullptr);

    #ifdef DEBUG
        DestroyDebugUtilsMessengerEXT(engine.instance, nullptr);
    #endif

    vkDestroySurfaceKHR(engine.instance, engine.surface, nullptr);
    vkDestroyInstance(engine.instance, nullptr);
}

void drawFrame
(
    EngineData          &engine,
    const UserSettings  &settings
){
    vkWaitForFences(engine.logicalDevice, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;

    VkResult result =
        vkAcquireNextImageKHR
        (
            engine.logicalDevice,
            engine.swapchain,
            UINT64_MAX,
            engine.acquireSemaphore,
            VK_NULL_HANDLE,
            &imageIndex
        );

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain(engine, settings);
        return;
    }
    else if(result != VK_SUBOPTIMAL_KHR)
    {
        riverAssertVkSuccess(result, "failed to acquire swapchain image!");
    }

    vkResetFences(engine.logicalDevice, 1, &engine.inFlightFences[currentFrame]);

    updateUniformBuffer(engine, currentFrame);

    vkResetCommandBuffer(engine.commandBuffers[currentFrame], 0);
    recordCommandBuffer(engine, engine.commandBuffers[currentFrame], imageIndex);

    std::array<VkSemaphore, 1> waitSemaphores =
    {
        engine.acquireSemaphore
    };

    std::array<VkSemaphore, 1> signalSemaphores =
    {
        engine.imageReadyForPresentSemaphores[imageIndex]
    };

    VkPipelineStageFlags waitStages[] =
    {
        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSubmitInfo drawSubmitInfo{};
    drawSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    drawSubmitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    drawSubmitInfo.pWaitSemaphores = waitSemaphores.data();
    drawSubmitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    drawSubmitInfo.pSignalSemaphores = signalSemaphores.data();

    drawSubmitInfo.pWaitDstStageMask = waitStages;
    drawSubmitInfo.commandBufferCount = 1;
    drawSubmitInfo.pCommandBuffers = &engine.commandBuffers[currentFrame];

    riverAssertVkSuccess
    (
        vkQueueSubmit(engine.graphicsQueue, 1, &drawSubmitInfo, engine.inFlightFences[currentFrame]),
        "failed to submit draw command buffer!"
    );

    VkSwapchainKHR swapchains[] =
    {
        engine.swapchain
    };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &engine.imageReadyForPresentSemaphores[imageIndex];

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    std::swap(engine.imageReadyForWriteSemaphores[imageIndex], engine.acquireSemaphore);

    result = vkQueuePresentKHR(engine.presentQueue, &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || engine.framebufferResized)
    {
        engine.framebufferResized = VK_FALSE;
        recreateSwapchain(engine, settings);
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
            riverLog(std::format("set project root to {}", current.string()), RIV_LOG_LEVEL_TRACE);
            return current;
        }
        current = current.parent_path();
    }
    return "RIV_PATH_UNDETERMINED";
}

void riverSetupLog(const std::filesystem::path &path)
{
#ifdef DEBUG
    return;
#endif
    logFile.open(path, std::ios::trunc);
    riverAssert(logFile.is_open(), "failed to open log file!");
}

void riverCloseLog()
{
#ifdef DEBUG
    return;
#endif
    if(logFile.is_open())
    {
        logFile.close();
    }
}

std::string riverTimestamp()
{
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm buf{};

#ifdef _WIN32
    localtime_s(&buf, &now);
#else
    localtime_r(&now, &buf);
#endif
    if ((buf.tm_hour > 23) || (buf.tm_hour < 0) ||
        (buf.tm_min  > 59) || (buf.tm_min  < 0) ||
        (buf.tm_sec  > 60) || (buf.tm_sec  < 0))
    {
      return "[??:??:??] ";
    }

    return std::format ("[{:02}:{:02}:{:02}] ", buf.tm_hour, buf.tm_min, buf.tm_sec);
}

void riverLog(const std::string_view text, const RiverLogLevel level)
{
    if(level < logLevel)
    {
        return;
    }

#ifdef DEBUG
    if(level == RIV_LOG_LEVEL_WARN || level == RIV_LOG_LEVEL_ERROR)
    {
        std::cerr << logLevelANSI[level] << riverTimestamp()
            << logLevelStamps[level] << text << clearANSI << '\n';
        return;
    }

    std::cout << logLevelANSI[level] << riverTimestamp()
        << logLevelStamps[level] << text << clearANSI << '\n';
#else

    if(level == RIV_LOG_LEVEL_WARN || level == RIV_LOG_LEVEL_ERROR)
    {
        std::cerr << riverTimestamp() << logLevelStamps[level] << text << '\n';
    }

    logFile << riverTimestamp() << logLevelStamps[level] << text << '\n' << std::flush;
#endif
}

void riverAssert(bool condition, const std::string_view assertFailureMsg)
{
    if(condition)
    {
        return;
    }

#ifdef DEBUG
    std::cerr << logLevelANSI[RIV_LOG_LEVEL_ASSERT] << riverTimestamp()
        << logLevelStamps[RIV_LOG_LEVEL_ASSERT] << assertFailureMsg << clearANSI << '\n';
#else
    logFile << riverTimestamp() << logLevelStamps[RIV_LOG_LEVEL_ASSERT] << assertFailureMsg << '\n' << std::flush;
#endif

    abort();
}

void riverAssertVkSuccess(VkResult result, const std::string_view assertFailureMsg)
{
    if(result == VK_SUCCESS)
    {
        return;
    }
#ifdef DEBUG
    std::cerr << logLevelANSI[RIV_LOG_LEVEL_ASSERT] << riverTimestamp() << logLevelStamps[RIV_LOG_LEVEL_ASSERT]
        << riverTranslateVkResult(result) << ": " << assertFailureMsg << clearANSI << '\n';

#else
    logFile << riverTimestamp() << logLevelStamps[RIV_LOG_LEVEL_ASSERT] << riverTranslateVkResult(result)
        << ": " <<  assertFailureMsg << '\n' << std::flush;
#endif

    abort();
}

void riverThrow(const std::string_view throwMsg)
{
    std::cerr << logLevelANSI[RIV_LOG_LEVEL_ASSERT] << riverTimestamp()
        << logLevelStamps[RIV_LOG_LEVEL_ASSERT] << ": " << throwMsg << clearANSI << '\n';
#ifndef DEBUG
    logFile << riverTimestamp() << logLevelStamps[RIV_LOG_LEVEL_ASSERT] << ": " << throwMsg << '\n' << std::flush;
#endif

    abort();
}
