#include "river.h"
#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

#include <cstring>
#include <iostream>
#include <fstream>

void river::printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines){
    if(build_DEBUG){
        if(firstOpen){
            remove(debugLog);
            firstOpen = false;
        }

        std::ofstream log(debugLog, std::ios::app);
        if(!log.is_open()){
            throw std::runtime_error("failed to open file");
        }

        for(;tabs > 0; --tabs){
            log << '\t'; 
        }

            log << text;

        for(;newlines > 0; --newlines){
            log << '\n';
        }

        log.close();
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType, 
        const VkDebugUtilsMessengerCallbackDataEXT  *pCallbackData,
        void                                        *pUserData
    ){

    std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';

    return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(
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

static std::vector<const char*> getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(build_DEBUG){
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); 
    }

    return extensions;
}

static bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt){
    using namespace river;
    printDebugLog("\nPresent:", 1, 1);
    for(const auto &extension : *instanceExt){
        printDebugLog(extension.extensionName, 2, 1);
    }

    printDebugLog("\nRequired:", 1, 1);
    for(const auto &required : *requiredExt){
        bool extFound = false;
        
            for(const auto &present : *instanceExt){
                if(0 == strcmp(required, present.extensionName)){
                    printDebugLog("found:\t", 0, 0);
                    printDebugLog(required, 1, 1);
                    extFound = true;
                    break;
                }
            }
        if(!extFound){ 
            printDebugLog("not found:\t", 0, 0);
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

    for(const char *layer : river::validationLayers){
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
    createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; 
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

static void setupDebugMessenger(){
    if(!build_DEBUG){ 
        return; 
    }
    using namespace river;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to set up debug messenger.", 2, 1);
        throw std::runtime_error("failed to set up debug messenger!");
    }else{
        printDebugLog("Successfully set up debug messenger.", 0, 1);
    }
}

static void DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(river::instance, "vkDestroyDebugUtilsMessengerEXT");
    if(nullptr != func){
        func(river::instance, messenger, pAllocator);
    }
}

static void createInstance(){
    if(build_DEBUG && !checkValidationLayerSupport()){
        river::printDebugLog("\nERROR: validation layers requested, but not available!", 2, 1);
        throw std::runtime_error("validation layers requested, but not available!");
    }
    using namespace river;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount); 
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data());

    std::vector<const char*> requiredExtensions = getRequiredExtensions();
    if(checkInstanceExtensions(&requiredExtensions, &instanceExtensions)){
        printDebugLog("\nAll required extensions are present.", 0, 1);
    }else{
        printDebugLog("\nERROR: extensions required, but not available!", 2, 1);
        throw std::runtime_error("extensions required, but not available!"); 
    }


    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.enabledLayerCount = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(build_DEBUG){
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); 
        createInfo.ppEnabledLayerNames = validationLayers.data();
        
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }else{
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create instance.", 2, 1);
        throw std::runtime_error("failed to create instance.");
    }else{
        printDebugLog("Successfully created instance.", 0, 1);
    }
}

void river::initVulkan() {

  createInstance();
  setupDebugMessenger();
  window::createSurface();
  device::pickPhysicalDevice();
  device::createLogicalDevice();
  swapchain::createSwapChain();
  swapchain::createImageViews();
  swapchain::createRenderPass();
  pipeline::createGraphicsPipeline();
  pipeline::createFramebuffers();
  pipeline::createCommandPool();
  pipeline::createCommandBuffer();
  pipeline::createSyncObjects();
}

void river::cleanupVulkan(){
    vkDeviceWaitIdle(device::logicalDevice);

    vkDestroySemaphore(device::logicalDevice, pipeline::imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device::logicalDevice, pipeline::renderFinishedSemaphore, nullptr);
    vkDestroyFence(device::logicalDevice, pipeline::inFlightFence, nullptr);

    vkDestroyCommandPool(device::logicalDevice, pipeline::commandPool, nullptr);

    for(const auto &framebuffer : swapchain::swapChainFramebuffers){
        vkDestroyFramebuffer(device::logicalDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(device::logicalDevice, pipeline::graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device::logicalDevice, pipeline::pipelineLayout, nullptr);
    vkDestroyRenderPass(device::logicalDevice, pipeline::renderPass, nullptr);

    for(const auto &imageView : swapchain::swapChainImageViews){
        vkDestroyImageView(device::logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device::logicalDevice, swapchain::swapChain, nullptr);
    vkDestroyDevice(device::logicalDevice, nullptr);

    if(build_DEBUG){
        DestroyDebugUtilsMessengerEXT(debugMessenger, nullptr); 
    }

    vkDestroySurfaceKHR(instance, window::surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void river::drawFrame(){
    uint32_t imageIndex;

    vkWaitForFences(device::logicalDevice, 1, &pipeline::inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device::logicalDevice, 1, &pipeline::inFlightFence);

    vkAcquireNextImageKHR(device::logicalDevice, swapchain::swapChain, UINT64_MAX, pipeline::imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(pipeline::commandBuffer, 0);
    pipeline::recordCommandBuffer(pipeline::commandBuffer, imageIndex);

    VkSemaphore waitSemaphores[] = {pipeline::imageAvailableSemaphore};
    VkSemaphore signalSemaphores[] = {pipeline::renderFinishedSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.signalSemaphoreCount = 1;

    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &pipeline::commandBuffer;

    if(vkQueueSubmit(pipeline::graphicsQueue, 1, &submitInfo, pipeline::inFlightFence) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to submit draw command buffer!", 2, 1);
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChains[] = {swapchain::swapChain};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(pipeline::presentQueue, &presentInfo);
}

//TODO: rewrite with recursion, base case is when the current path is the drive root, throw runtime error there
void river::getProjectRoot(const char *rootName){
    std::filesystem::path current = std::filesystem::current_path();

    for(int i = 0; i < 3; ++i){
        if(strcmp(current.filename().string().c_str(), rootName) == 0) {
            appRoot = current;
            debugLog = current / "debug.log";
            printDebugLog("project root:", 0, 0);
            printDebugLog(current.string(), 1, 1);
            return;
        }else{
            current = current.parent_path();
        }
    }
}
