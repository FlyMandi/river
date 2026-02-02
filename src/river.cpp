#include "vulkan/vulkan_core.h"
#include "river.h"
#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffer.h"

#include <cstdint>
#include <cstring>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT  *callbackData,
    void                                        *userData
){
    riverLog("VL says:", RIV_LOG_LEVEL_DEBUG);
    riverLog(callbackData->pMessage, RIV_LOG_LEVEL_DEBUG);
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
    riverLog("Present:", RIV_LOG_LEVEL_DEBUG);
    for(const auto &extension : *instanceExt)
    {
        riverLog(extension.extensionName, RIV_LOG_LEVEL_DEBUG);
    }
    riverLog("Required:", RIV_LOG_LEVEL_DEBUG);

    for(const auto &required : *requiredExt)
    {
        VkBool32 extFound = VK_FALSE;
        
            for(const auto &present : *instanceExt)
            {
                if(0 == strcmp(required, present.extensionName))
                {
                    riverLog(required, RIV_LOG_LEVEL_DEBUG);
                    extFound = VK_TRUE;
                    break;
                }
            }
        if(!extFound)
        {
            riverLog("extension not found: ", RIV_LOG_LEVEL_WARN);
            riverLog(required, RIV_LOG_LEVEL_WARN);

            return VK_FALSE; 
        } 
    }
    return VK_TRUE;
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
            riverLog("validation layer not found: ", RIV_LOG_LEVEL_WARN);
            riverLog(layer, RIV_LOG_LEVEL_WARN);
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

#ifdef DEBUG
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

static void createInstance()
{
    #ifdef DEBUG
        riverAssert(checkValidationLayerSupport(), "validation layers requested, but not available!");
    #endif

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
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

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    #ifdef DEBUG
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
    createDescriptorPool();
    createDescriptorSets();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPools();

    createVertexBuffer();
    createUniformBuffers();
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
    
    //OPTIM: do we need to wait for fences here?
    vkWaitForFences(logicalDevice, 1, inFlightFences.data(), VK_TRUE, UINT64_MAX);

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
    uint32_t imageIndex;

    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    destroyDeferredResources(&frameResources[currentFrame]);

    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain();
        return;
    }
    else 
    {
        riverAssertVkSuccess(result, "failed to acquire swapchain image!");
    }

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    updateUniformBuffer(currentFrame);

    VkSubmitInfo drawSubmitInfo{};
    drawSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    drawSubmitInfo.pWaitSemaphores = waitSemaphores;
    drawSubmitInfo.waitSemaphoreCount = 1;
    drawSubmitInfo.pSignalSemaphores = signalSemaphores;
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
    presentInfo.pWaitSemaphores = signalSemaphores;

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

//HACK: only checks up to 4 paths up. jank.
std::filesystem::path getProjectRoot(const char *rootName)
{
    std::filesystem::path current = std::filesystem::current_path();

    for(int i = 0; i < 4; ++i)
    {
        if(strcmp(current.filename().string().c_str(), rootName) == 0)
        {
            riverLog("project root: ", RIV_LOG_LEVEL_DEBUG);
            riverLog(current, RIV_LOG_LEVEL_DEBUG);
            return current;
        }
        current = current.parent_path();
    }
    return ".";
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

//TODO:#43: write logs & asserts to a file in release builds AND
//show an actual useful runtime error message box, not just "abort has been called"
void riverLog(const auto &text, const RiverLogLevel level)
{
    if(level < logLevel)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    switch(level)
    {
        case RIV_LOG_LEVEL_TRACE:
            std::cout << std::put_time(&buf, "[%T]-") << "[RIV_TRACE]: " << text << '\n';
            return;

        case RIV_LOG_LEVEL_DEBUG:
            std::cout << std::put_time(&buf, "[%T]-") << "[RIV_DEBUG]: " << text << '\n';
            return;

        case RIV_LOG_LEVEL_WARN:
            std::cerr << std::put_time(&buf, "[%T]-") << "[RIV_WARN]:  " << text << '\n';
            return;

        case RIV_LOG_LEVEL_ERROR:
            std::cerr << std::put_time(&buf, "[%T]-") << "[RIV_ERROR]: " << text << '\n';
            return;

        case RIV_LOG_LEVEL_ASSERT:
            std::cerr << std::put_time(&buf, "[%T]-") << "[RIV_ASSERT]: " << text << '\n';
            abort();
    }
}

//FIXME:#44: WIP
void riverAssert(bool condition, const auto &assertFailureMsg)
{
    if(condition)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << std::put_time(&buf, "[%T]-") << "[RIV_ASSERT]: " << assertFailureMsg << '\n';
    abort();
}

void riverAssertVkSuccess(VkResult result, const auto &assertFailureMsg)
{
    if(result == VK_SUCCESS)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << std::put_time(&buf, "[%T]-") << "[RIV_ASSERT]: " << result << ": " << assertFailureMsg << '\n';
    abort();
}
