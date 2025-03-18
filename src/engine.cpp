#include "river.h"
#include "window.h"
#include "engine.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

#include "GLFW/glfw3.h"

#include <iostream>

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

static bool checkValidationLayerSupport(){
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> layerVec(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerVec.data());

    for(const char *layer : device::validationLayers){
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

static bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt){
    using namespace engine;

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

void engine::createInstance(){
    if(build_DEBUG && !checkValidationLayerSupport()){
        printDebugLog("\nERROR: validation layers requested, but not available!", 2, 1);
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = river::appName;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = river::engineName;
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
        createInfo.enabledLayerCount = static_cast<uint32_t>(device::validationLayers.size()); 
        createInfo.ppEnabledLayerNames = device::validationLayers.data();
        
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

void engine::setupDebugMessenger(){
    if(!build_DEBUG){ 
        return; 
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &device::debugMessenger) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to set up debug messenger.", 2, 1);
        throw std::runtime_error("failed to set up debug messenger!");
    }else{
        printDebugLog("Successfully set up debug messenger.", 0, 1);
    }
}

void device::DestroyDebugUtilsMessengerEXT(
        VkInstance                  instance,
        VkDebugUtilsMessengerEXT    messenger, 
        const VkAllocationCallbacks *pAllocator
    ){

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(nullptr != func){
        func(instance, messenger, pAllocator);
    }
}

void engine::createSurface(){
    if(glfwCreateWindowSurface(instance, window::pWindow, nullptr, &swap::surface) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create window surface!", 2, 1);
        throw std::runtime_error("failed to create window surface!");
    }else{
        printDebugLog("Successfully created window surface.", 0, 1);
    }
}

void engine::createFramebuffers(){
    using namespace swap;

    swapChainFramebuffers.resize(swapChainImageViews.size());

    for(size_t i = 0; i < swapChainImageViews.size(); ++i){
        VkImageView attachments[] = { swapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipeline::renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(device::logicalDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS){
            printDebugLog("\nERROR: failed to create framebuffer!", 2, 1);
            throw std::runtime_error("failed to create framebuffer!");
        }else{
            printDebugLog("Successfully created framebuffer.", 0, 1);
        }
    }
}

void engine::createCommandPool(){
    using namespace device;

    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if((vkCreateCommandPool(device::logicalDevice, &poolInfo, nullptr, &pipeline::commandPool)) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create command pool!", 2, 1);
        throw std::runtime_error("failed to create command pool!");
    }else{
        printDebugLog("Successfully created command pool.", 0, 1);
    }
}

void engine::createCommandBuffer(){
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pipeline::commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if(vkAllocateCommandBuffers(device::logicalDevice, &allocInfo, &pipeline::commandBuffer) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to allocate command buffers!", 2, 1);
        throw std::runtime_error("failed to allocate command buffers!");
    }else{
        printDebugLog("Successfully allocated command buffer.", 0, 1);
    }
}

void engine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex){
    using namespace swap;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if((vkBeginCommandBuffer(commandBuffer, &beginInfo)) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to begin recording command buffer!", 2, 1);
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipeline::renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline::graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    //NOTE: I'm specifyin the num of verts... lmfao
    vkCmdDraw(commandBuffer, 3, 2, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to record command buffer!", 2, 1);
        throw std::runtime_error("failed to record command buffer!");
    }
}

void engine::createSyncObjects(){
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if((vkCreateSemaphore(device::logicalDevice, &semaphoreInfo, nullptr, &pipeline::imageAvailableSemaphore)) != VK_SUCCESS || (vkCreateSemaphore(device::logicalDevice, &semaphoreInfo, nullptr, &pipeline::renderFinishedSemaphore)) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create semaphores!", 2, 1);
        throw std::runtime_error("failed to create semaphores!");
    }else{
        printDebugLog("Successfully created semaphores.", 0, 1);
    }
    if((vkCreateFence(device::logicalDevice, &fenceInfo, nullptr, &pipeline::inFlightFence)) != VK_SUCCESS){
        printDebugLog("\nERROR: failed to create fence!", 2, 1);
        throw std::runtime_error("failed to create fence!");
    }else{
        printDebugLog("Successfully created fence.", 0, 1);
    }
}
