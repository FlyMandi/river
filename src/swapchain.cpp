#include "vulkan/vulkan_core.h"

#include "river.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffer.h"
#include "image.h"

#include <algorithm>
#include <cstdint>

internal VkExtent2D chooseSwapExtent
(
    const VkSurfaceCapabilitiesKHR  &capabilities,
    GLFWwindow                      *window
){
    if(std::numeric_limits<uint32_t>::max() != capabilities.currentExtent.width)
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void createSwapchain
(
    const EngineData    &engine,
    const UserSettings  &settings
){
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, engine.surface);
    VkSurfaceFormatKHR surfaceFormat = swapchainSupport.formats[0];

    for(const auto &availableFormat : swapchainSupport.formats)
    {
        if( VK_FORMAT_B8G8R8A8_SRGB == availableFormat.format &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        ){
            surfaceFormat = availableFormat;
        }
    }

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; //fallback

    for(const auto &availablePresentMode : swapchainSupport.presentModes)
    {
        if(availablePresentMode == settings.presentMode)
        {
            presentMode = availablePresentMode;
        }
    }

    VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities, engine.window);

    swapchainImageCount = swapchainSupport.capabilities.minImageCount + 1;

    if( 0 < swapchainSupport.capabilities.maxImageCount &&
        swapchainImageCount > swapchainSupport.capabilities.maxImageCount
    ){
        swapchainImageCount = swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = engine.surface;
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

    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
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

void createSwapImageViews()
{
    swapchainImageViews.resize(swapchainImages.size());

    for(uint32_t i = 0; i < swapchainImages.size(); ++i)
    {
        swapchainImageViews[i] =    createImageView
                                    (
                                        swapchainImages[i],
                                        swapchainImageFormat,
                                        VK_IMAGE_ASPECT_COLOR_BIT
                                    );
    }
}

void createRenderPass()
{
    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.format = swapchainImageFormat;
    colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    const std::vector<VkFormat> candidates =
    {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    VkAttachmentDescription depthAttachmentDescription{};
    depthAttachmentDescription.format = findSupportedFormat
                                        (
                                            candidates,
                                            VK_IMAGE_TILING_OPTIMAL,
                                            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                                        );
    depthAttachmentDescription.samples  = VK_SAMPLE_COUNT_1_BIT;
    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment= 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask =   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT   |
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask =   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT   |
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT            |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments =
    {
        colorAttachmentDescription,
        depthAttachmentDescription
    };

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    riverAssertVkSuccess
    (
        vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderPass),
        "failed to create render pass!"
    );
}

void cleanupSwapchain()
{
    vkDestroyImageView(logicalDevice, depthImageView, nullptr);
    vkDestroyImage(logicalDevice, depthImage, nullptr);
    vkFreeMemory(logicalDevice, depthImageMemory, nullptr);

    for(size_t i = 0; i < swapchainFramebuffers.size(); ++i)
    {
        vkDestroyFramebuffer(logicalDevice, swapchainFramebuffers[i], nullptr);
    }

    for(size_t i = 0; i < swapchainImageViews.size(); ++i)
    {
        vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
}

void recreateSwapchain
(
    const EngineData    &engine,
    const UserSettings  &settings
){
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(engine.window, &width, &height);
    while(width == 0 || height == 0)
    {
        glfwGetFramebufferSize(engine.window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(logicalDevice);

    cleanupSwapchain();

    createSwapchain(engine, settings);
    createSwapImageViews();
    createDepthResources();
    createFramebuffers();

    riverLog(std::format("recreated swapchain: {}x{}", width, height), RIV_LOG_LEVEL_TRACE);
}
