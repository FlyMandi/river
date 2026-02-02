#include "river.h"
#include "vulkan/vulkan_core.h"
#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

#include <algorithm>

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats){
    for(const auto &availableFormat : availableFormats)
    {
        if( VK_FORMAT_B8G8R8_SRGB == availableFormat.format && 
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == availableFormat.colorSpace
        ){
            return availableFormat;
        }
    }
    
    return availableFormats[0];
}

static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes){
    for(const auto &availablePresentMode : availablePresentModes)
    {
        if(VK_PRESENT_MODE_IMMEDIATE_KHR == availablePresentMode)
        {
            #ifdef DEBUG
                riverLog("present mode: VK_PRESENT_MODE_IMMEDIATE_KHR", RIV_LOG_LEVEL_DEBUG);
            #endif
            return availablePresentMode;
        }
    }

    for(const auto &availablePresentMode : availablePresentModes)
    {
        if(VK_PRESENT_MODE_MAILBOX_KHR == availablePresentMode)
        {
            #ifdef DEBUG
                riverLog("present mode: VK_PRESENT_MODE_MAILBOX_KHR", RIV_LOG_LEVEL_DEBUG);
            #endif
            return availablePresentMode;
        }
    }

    riverLog("present mode: VK_PRESENT_MODE_FIFO_KHR", RIV_LOG_LEVEL_DEBUG);
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if(std::numeric_limits<uint32_t>::max() != capabilities.currentExtent.width)
    {
        riverLog("swap width: ", RIV_LOG_LEVEL_TRACE);
        riverLog(capabilities.currentExtent.width, RIV_LOG_LEVEL_TRACE);
        riverLog("swap height: ", RIV_LOG_LEVEL_TRACE);
        riverLog(capabilities.currentExtent.height, RIV_LOG_LEVEL_TRACE);

        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        riverLog("swap width (clamped): ", RIV_LOG_LEVEL_TRACE);
        riverLog(actualExtent.width, RIV_LOG_LEVEL_TRACE);
        riverLog("swap height (clamped): ", RIV_LOG_LEVEL_TRACE);
        riverLog(actualExtent.height, RIV_LOG_LEVEL_TRACE);
        
        return actualExtent;
    }
}

void createSwapchain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    swapchainImageCount = swapChainSupport.capabilities.minImageCount + 1;

    if( 0 < swapChainSupport.capabilities.maxImageCount && 
        swapchainImageCount > swapChainSupport.capabilities.maxImageCount
    ){
        swapchainImageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = swapchainImageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent; 
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {
        logicalQueueFamilies.graphicsIndex, 
        logicalQueueFamilies.presentIndex
    };

    if(logicalQueueFamilies.graphicsIndex != logicalQueueFamilies.presentIndex)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = sizeof(queueFamilyIndices)/sizeof(uint32_t);
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    riverAssertVkSuccess
    (
        vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapchain),
        "failed to create swap chain!"
    );

    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount, nullptr);
    swapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount, swapchainImages.data());

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;
}

void createImageViews()
{
    swapchainImageViews.resize(swapchainImages.size()); 

    for(size_t i = 0; i < swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        riverAssertVkSuccess
        (
            vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapchainImageViews[i]),
            "failed to create image views!"
        );
    }
}

void createRenderPass()
{
    VkAttachmentDescription colorAttachment{}; 
    colorAttachment.format = swapchainImageFormat;
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

    riverAssertVkSuccess
    (
        vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass),
        "failed to create render pass!"
    );
}

void destroyDeferredResources(FrameResource *frame)
{
    if(frame->hasFramebuffer)
    {
        vkDestroyFramebuffer(logicalDevice, frame->framebuffer2Destroy, nullptr);
        frame->hasFramebuffer = VK_FALSE;
    }

    if(frame->hasImageView)
    {
        vkDestroyImageView(logicalDevice, frame->imageView2Destroy, nullptr);
        frame->hasImageView = VK_FALSE;
    }
};

void cleanupSwapchain()
{
    for(size_t i = 0; i < swapchainImageCount; ++i)
    {
        frameResources[i].framebuffer2Destroy = swapchainFramebuffers[i];
        frameResources[i].imageView2Destroy = swapchainImageViews[i];
        destroyDeferredResources(&frameResources[i]);
    }

    vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
}

void recreateSwapchain()
{
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window, &width, &height);
    while(width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    //TODO:#38: fence
    vkDeviceWaitIdle(logicalDevice);

    cleanupSwapchain();

    createSwapchain();
    createImageViews();
    createFramebuffers();

    riverLog("swap recreated.", RIV_LOG_LEVEL_DEBUG);
}
