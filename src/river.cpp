#include "river.h"
#include "vulkan/vulkan_core.h"
#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType, 
        const VkDebugUtilsMessengerCallbackDataEXT  *callbackData,
        void                                        *userData
    ){

    printDebugLog('\0', "[VL] ");
    printDebugLog(callbackData->pMessage, '\n');

    return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(
        VkInstance                                  instance,
        const VkDebugUtilsMessengerCreateInfoEXT    *createInfo,
        const VkAllocationCallbacks                 *allocator,
        VkDebugUtilsMessengerEXT                    *debugMessenger
    ){

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(nullptr != func){
        return func(instance, createInfo, allocator, debugMessenger);
    }else{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static std::vector<const char*> getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(BUILD_DEBUG){
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); 
    }

    return extensions;
}

static bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt){
    printDebugLog('\0', "Present:", '\n');
    for(const auto &extension : *instanceExt){
        printDebugLog('\t', extension.extensionName, '\n');
    }

    printDebugLog('\0', "Required:", '\n');
    for(const auto &required : *requiredExt){
        bool extFound = false;
        
            for(const auto &present : *instanceExt){
                if(0 == strcmp(required, present.extensionName)){
                    printDebugLog('\t', required, '\n');
                    extFound = true;
                    break;
                }
            }
        if(!extFound){ 
            printDebugLog('\0', "!!!", '\t');
            printDebugLog(required, '\n');
            return false; 
        } 
    }

    return true;
}

static bool checkValidationLayerSupport(){
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
        if(!layerFound){
            return false; 
        }
    }

    return true;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
    createInfo = {}; 
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; 
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

static void setupDebugMessenger(){
    if(!BUILD_DEBUG){ 
        return; 
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS){
        printDebugLog('\n', "ERROR: failed to set up debug messenger.");
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

static void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks *allocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(nullptr != func){
        func(instance, debugMessenger, allocator);
    }
}

static void createInstance(){
    if(BUILD_DEBUG && !checkValidationLayerSupport()){
        printDebugLog('\n', "ERROR: validation layers requested, but not available!");
        throw std::runtime_error("validation layers requested, but not available!");
    }

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
    if(!checkInstanceExtensions(&requiredExtensions, &instanceExtensions)){
        printDebugLog('\n', "ERROR: extensions required, but not available!");
        throw std::runtime_error("extensions required, but not available!"); 
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.enabledLayerCount = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(BUILD_DEBUG){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); 
        createInfo.ppEnabledLayerNames = validationLayers.data();
        
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }else{
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        printDebugLog('\n', "ERROR: failed to create instance.");
        throw std::runtime_error("failed to create instance.");
    }
}

void initVulkan(){
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void cleanupVulkan(){
    vkDeviceWaitIdle(logicalDevice);

    cleanupSwapChain();

    vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i){
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);

    if(BUILD_DEBUG){
        DestroyDebugUtilsMessengerEXT(nullptr); 
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void drawFrame(){
    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR){
        recreateSwapChain();
        return;

    }else if(result != VK_SUCCESS){
        printDebugLog("failed to acquire swapChain image!");
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

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to submit draw command buffer!");
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChains[] = {swapChain};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){
        recreateSwapChain();

    }else if(result != VK_SUCCESS){
        printDebugLog("failed to present swapChain image!");
        throw std::runtime_error("failed to present swapChain image!");
    }

    currentFrame = (++currentFrame) % MAX_FRAMES_IN_FLIGHT;
}

//TODO: rewrite with recursion, base case is when the current path is the drive root, throw runtime error there
std::filesystem::path getProjectRoot(const char *rootName){
    std::filesystem::path current = std::filesystem::current_path();

    for(int i = 0; i < 3; ++i){
        if(strcmp(current.filename().string().c_str(), rootName) == 0){
            printDebugLog('\0', "project root: ");
            printDebugLog(current, '\n');
            return current;
        }else{
            current = current.parent_path();
        }
    }
    return ".";
}

void clearLogs(const std::filesystem::path &baseDir){
    for(const auto &log : std::filesystem::directory_iterator(baseDir)){
        if(".log" == log.path().extension()){
            std::filesystem::remove(log);
        }
    }
}
