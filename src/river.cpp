#include "river.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

#if defined(DEBUG) || defined (_DEBUG)
#include "debugger.h"
#endif

void River::initVulkan(){

    createInstance();

#if defined(DEBUG) || defined (_DEBUG)
        debugger.setupDebugMessenger(instance);
#endif

    window.createSurface();
    device.pickPhysicalDevice();
    device.createLogicalDevice();
    swapchain.createSwapChain();
    swapchain.createImageViews();
    swapchain.createRenderPass();
    pipeline.createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

void River::cleanupVulkan(){
    vkDeviceWaitIdle(device.logicalDevice);

    vkDestroySemaphore(device.logicalDevice, pipeline.imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device.logicalDevice, pipeline.renderFinishedSemaphore, nullptr);
    vkDestroyFence(device.logicalDevice, pipeline.inFlightFence, nullptr);

    vkDestroyCommandPool(device.logicalDevice, pipeline.commandPool, nullptr);

    for(const auto &framebuffer : swapchain.swapChainFramebuffers){
        vkDestroyFramebuffer(device.logicalDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(device.logicalDevice, pipeline.graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.logicalDevice, pipeline.pipelineLayout, nullptr);
    vkDestroyRenderPass(device.logicalDevice, pipeline.renderPass, nullptr);

    for(const auto &imageView : swapchain.swapChainImageViews){
        vkDestroyImageView(device.logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device.logicalDevice, swapchain.swapChain, nullptr);
    vkDestroyDevice(device.logicalDevice, nullptr);

    if(build_DEBUG){
        DestroyDebugUtilsMessengerEXT(instance, debugger.debugMessenger, nullptr); 
    }

    vkDestroySurfaceKHR(instance, swapchain.surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void River::drawFrame(){
    uint32_t imageIndex;

    vkWaitForFences(device.logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device.logicalDevice, 1, &inFlightFence);

    vkAcquireNextImageKHR(device.logicalDevice, swap.swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    
    vkResetCommandBuffer(commandBuffer, 0);
    engine.recordCommandBuffer(commandBuffer, imageIndex);

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.signalSemaphoreCount = 1;

    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &pipeline.commandBuffer;

    if(vkQueueSubmit(engine.graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS){
        engine.printDebugLog("\nERROR: failed to submit draw command buffer!", 2, 1);
        throw std.runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChains[] = {swap.swapChain};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(engine.presentQueue, &presentInfo);
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

static bool checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt){
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


void engine::createFramebuffers(){
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

//TODO: rewrite with recursion, base case is when the current path is the drive root, throw runtime error there
void River::getProjectRoot(const char *rootName){
    std::filesystem::path current = std::filesystem::current_path();

    for(int i = 0; i < 3; ++i){
        if(strcmp(current.filename().string().c_str(), rootName) == 0) {
            appRoot = current;
            debugger.debugLog = current / "debug.log";
            debugger.printDebugLog("project root:", 0, 0);
            debugger.printDebugLog(current.string(), 1, 1);
            return;
        }else{
            current = current.parent_path();
        }
    }
}

