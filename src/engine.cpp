#include "river.h"

#include "h/engine.h"

void engine::createInstance(){
    if(build_DEBUG && !checkValidationLayerSupport()){
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
    if(!checkInstanceExtensions(&requiredExtensions, &instanceExtensions)){
        throw std::runtime_error("extensions required, but not available!"); 
    }

    printDebugLog("\nAll required extensions are present.", 0, 2);

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

    if(vkCreateInstance(&createInfo, nullptr, &instance)){
        throw std::runtime_error("failed to create instance.");
    }
}

void engine::setupDebugMessenger(){
    if(!build_DEBUG){ 
        return; 
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)){
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult engine::CreateDebugUtilsMessengerEXT(
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

void engine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
    createInfo = {}; 
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; 
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void engine::DestroyDebugUtilsMessengerEXT(
        VkInstance                  instance,
        VkDebugUtilsMessengerEXT    messenger, 
        const VkAllocationCallbacks *pAllocator
    ){

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(nullptr != func){
        func(instance, messenger, pAllocator);
    }
}

bool engine::checkValidationLayerSupport(){
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

std::vector<const char*> engine::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(build_DEBUG){
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); 
    }

    return extensions;
}

bool engine::checkInstanceExtensions(std::vector<const char*> *requiredExt, std::vector<VkExtensionProperties> *instanceExt){
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

void engine::createSurface(){
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("failed to create window surface!");
    }
}

void engine::createImageViews(){
    swapChainImageViews.resize(swapChainImages.size()); 

    for(size_t i = 0; i < swapChainImages.size(); ++i){
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void engine::createRenderPass(){
    VkAttachmentDescription colorAttachment{}; 
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if((vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass)) != VK_SUCCESS){
        throw std::runtime_error("failed to create render pass!");
    }
}

void engine::createFramebuffers(){
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for(size_t i = 0; i < swapChainImageViews.size(); ++i){
        VkImageView attachments[] = { swapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void engine::createCommandPool(){
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if((vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool)) != VK_SUCCESS){
        throw std::runtime_error("failed to create command pool!");
    }
}

void engine::createCommandBuffer(){
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void engine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if((vkBeginCommandBuffer(commandBuffer, &beginInfo)) != VK_SUCCESS){
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

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
        throw std::runtime_error("failed to record command buffer!");
    }
}

void engine::createSyncObjects(){
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if((vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore)) != VK_SUCCESS || (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore)) != VK_SUCCESS){
        throw std::runtime_error("failed to create semaphores!");
    }
    if((vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence)) != VK_SUCCESS){
        throw std::runtime_error("failed to create fence!");
    }
}
