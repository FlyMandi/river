#include "vulkan/vulkan_core.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "river.h"
#include "device.h"
#include "pipeline.h"
#include "buffer.h"
#include "image.h"

static void copyBufferToImage
(
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height
){
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset =
    {
        0, 0, 0
    };
    region.imageExtent =
    {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage
    (
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(commandBuffer);
}

static void transitionImageLayout
(
    VkImage         image,
    VkFormat        format,
    VkImageLayout   oldLayout,
    VkImageLayout   newLayout
){
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    //TODO: fixup transitionImageLayout:
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier
    (
        commandBuffer,
        0, //TODO
        0, //TODO
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void createTextureImage()
{
    int texWidth;
    int texHeight;
    int texChannels;

    stbi_uc *pixels =   stbi_load
                        (
                            "assets/textures/texture.jpg",
                            &texWidth,
                            &texHeight,
                            &texChannels,
                            STBI_rgb_alpha
                        );

    riverAssert(pixels, "failed to load texture image!");

    VkDeviceSize imageSize = texWidth * texHeight * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    std::set<uint32_t> uniqueFamilyIndices =
    {
        logicalQueueFamilies.graphicsIndex,
        logicalQueueFamilies.transferIndex
    };

    createBuffer
    (
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory,
        uniqueFamilyIndices
    );

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    ::memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage
    (
        texWidth,
        texHeight,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        textureImage,
        textureImageMemory
    );

    transitionImageLayout
    (
        textureImage,
        VK_FORMAT_R8G8B8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    copyBufferToImage
    (
        stagingBuffer,
        textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight)
    );

    transitionImageLayout
    (
        textureImage,
        VK_FORMAT_R8G8B8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
    );
}

void createImage
(
    uint32_t                width,
    uint32_t                height,
    VkFormat                format,
    VkImageTiling           tiling,
    VkImageUsageFlags       usage,
    VkMemoryPropertyFlags   memPropFlags,
    VkImage                 &image,
    VkDeviceMemory          &imageMem
){
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;

    //in an edge case, this format might not be supported. conversions will be done eventually
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.flags = 0;

    riverAssertVkSuccess
    (
        vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &image),
        "failed to create image!"
    );

    VkMemoryRequirements imageMemRequirements;
    vkGetImageMemoryRequirements(logicalDevice, image, &imageMemRequirements);

    VkMemoryAllocateInfo mAllocInfo{};
    mAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mAllocInfo.allocationSize = imageMemRequirements.size;
    mAllocInfo.memoryTypeIndex = findSuitableMemoryType(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    riverAssertVkSuccess
    (
        vkAllocateMemory(logicalDevice, &mAllocInfo, nullptr, &imageMem),
        "failed to allocate image memory!"
    );

    vkBindImageMemory(logicalDevice, image, imageMem, 0);
}
