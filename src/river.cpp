#include "vulkan/vulkan_core.h"
#include "river.h"
#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffer.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT  *callbackData,
    void                                        *userData
){
    #ifdef DEBUG
        printDebugLog('\0', "[VL] ");
        printDebugLog(callbackData->pMessage, '\n');
    #endif

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

static bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt)
{
    #ifdef DEBUG
        printDebugLog('\0', "Present:", '\n');
        for(const auto &extension : *instanceExt)
        {
            printDebugLog('\t', extension.extensionName, '\n');
        }

        printDebugLog('\0', "Required:", '\n');
    #endif

    for(const auto &required : *requiredExt)
    {
        bool extFound = false;
        
            for(const auto &present : *instanceExt)
            {
                if(0 == strcmp(required, present.extensionName))
                {
                    #ifdef DEBUG
                        printDebugLog('\t', required, '\n');
                    #endif

                    extFound = true;
                    break;
                }
            }
        if(!extFound)
        {
            #ifdef DEBUG
                printDebugLog('\0', "!!!", '\t');
                printDebugLog(required, '\n');
            #endif

            return false; 
        } 
    }
    return true;
}

static bool checkValidationLayerSupport()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> layerVec(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerVec.data());

    for(const char *layer : validationLayers)
    {
        bool layerFound = false;

        for(const auto &layerPresent : layerVec)
        {
            if(0 == strcmp(layerPresent.layerName, layer))
            {
                layerFound = true;
                break;
            }
        }
        if(!layerFound)
        {
            return false; 
        }
    }

    return true;
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

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        printDebugLog("failed to set up debug messenger.");
        throw std::runtime_error("failed to set up debug messenger!");
    }
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
        if(!checkValidationLayerSupport())
        {
            printDebugLog("validation layers requested, but not available!");
            throw std::runtime_error("validation layers requested, but not available!");
        }
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
    if(!checkInstanceExtensions(&requiredExtensions, &instanceExtensions))
    {
        #ifdef DEBUG
            printDebugLog("extensions required, but not available!");
        #endif
        throw std::runtime_error("extensions required, but not available!"); 
    }

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

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        #ifdef DEBUG
            printDebugLog("failed to create instance.");
        #endif
        throw std::runtime_error("failed to create instance.");
    }
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
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPools();
    createVertexBuffer();
    createCommandBuffers();
    createSyncObjects();
}

void cleanupVulkan()
{
    //TODO:#38: fences.
    vkDeviceWaitIdle(logicalDevice);

    cleanupSwapChain();

    vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);

    vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
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
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }
    else if(result != VK_SUCCESS)
    {
        #ifdef DEBUG
            printDebugLog("failed to acquire swapChain image!");
        #endif
        throw std::runtime_error("failed to acquire swapChain image!");
    }

    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.signalSemaphoreCount = 1;

    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        #ifdef DEBUG
            printDebugLog("failed to submit draw command buffer!");
        #endif
        throw std::runtime_error("failed to submit draw command buffer!");
    }

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
        framebufferResized = false;
        recreateSwapChain();
    }
    else if(result != VK_SUCCESS)
    {
        #ifdef DEBUG
            printDebugLog("failed to present swapChain image!");
        #endif
        throw std::runtime_error("failed to present swapChain image!");
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
            #ifdef DEBUG
                printDebugLog('\0', "project root: ");
                printDebugLog(current, '\n');
            #endif
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
