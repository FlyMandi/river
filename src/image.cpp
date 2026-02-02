#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "vulkan/vulkan_core.h"

#include "river.h"
#include "pipeline.h"
#include "buffer.h"
#include "image.h"

internal void copyBufferToImage
(
    EngineData          &engine,
    const VkBuffer      &buffer,
    const VkImage       &image,
    const uint32_t      &width,
    const uint32_t      &height
){
    VkCommandBuffer commandBuffer = setupCommandBuffer(engine, engine.graphicsCommandPool);

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
        buffer, image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    flushCommandBuffer(engine, commandBuffer, engine.graphicsCommandPool, engine.graphicsQueue);
}

void transitionImageLayout
(
    EngineData          &engine,
    const VkImage       &image,
    const uint32_t      &mipLevels,
    const VkFormat      &format,
    const VkImageLayout &oldLayout,
    const VkImageLayout &newLayout
){
    VkCommandBuffer commandBuffer = setupCommandBuffer(engine, engine.graphicsCommandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;

    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if( format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
            format == VK_FORMAT_D24_UNORM_S8_UINT)
        {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        riverThrow("unsupported layout transition!");
    }

    vkCmdPipelineBarrier
    (
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    flushCommandBuffer(engine, commandBuffer, engine.graphicsCommandPool, engine.graphicsQueue);
}

internal void generateMipmaps
(
    EngineData          &engine,
    const VkImage       &image,
    const uint32_t      &mipLevels,
    const int32_t       &texWidth,
    const int32_t       &texHeight
){
    VkCommandBuffer commandBuffer = setupCommandBuffer(engine, engine.graphicsCommandPool);

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for(uint32_t i = 1; i < mipLevels; ++i)
    {
        imageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier
        (
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr,
            1, &imageMemoryBarrier
        );

        VkImageBlit imageBlit{};
        imageBlit.srcOffsets[0] = {0, 0, 0};
        imageBlit.srcOffsets[1] =
        {
            mipWidth,
            mipHeight,
            1
        };
        imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.srcSubresource.mipLevel = i - 1;
        imageBlit.srcSubresource.baseArrayLayer = 0;
        imageBlit.srcSubresource.layerCount = 1;
        imageBlit.dstOffsets[0] = {0, 0, 0};
        imageBlit.dstOffsets[1] =
        {
            mipWidth  > 1 ? mipWidth  / 2 : 1,
            mipHeight > 1 ? mipHeight / 2 : 1,
            1
        };
        imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.dstSubresource.mipLevel = i;
        imageBlit.dstSubresource.baseArrayLayer = 0;
        imageBlit.dstSubresource.layerCount = 1;

        vkCmdBlitImage
        (
            commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &imageBlit,
            VK_FILTER_LINEAR
        );

        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier
        (
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, nullptr, 0, nullptr,
            1, &imageMemoryBarrier
        );

        if(mipWidth > 1)
        {
            mipWidth /= 2;
        }

        if(mipHeight > 1)
        {
            mipHeight /= 2;
        }
    }

    imageMemoryBarrier.subresourceRange.baseMipLevel = mipLevels - 1;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier
    (
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr,
        1, &imageMemoryBarrier
    );

    flushCommandBuffer(engine, commandBuffer, engine.graphicsCommandPool, engine.graphicsQueue);
}

void createTextureImage
(
    EngineData              &engine,
    const ProjectManifest   &manifest
){
    int texWidth;
    int texHeight;
    int texChannels;

    stbi_uc *pixels =   stbi_load
                        (
                            manifest.projectTexturePath.string().c_str(),
                            &texWidth,
                            &texHeight,
                            &texChannels,
                            STBI_rgb_alpha
                        );

    engine.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    const char* stbi_error = stbi_failure_reason();

    if(!pixels)
    {
        riverThrow
        (
            std::format
            (
                "failed to load texture image from {}: {}",
                manifest.projectTexturePath.string(), stbi_failure_reason()
            )
        );
    }

    VkDeviceSize imageSize = texWidth * texHeight * 4;

    std::set<uint32_t> uniqueFamilyIndices =
    {
        engine.logicalQueueFamilies.graphicsIndex,
        engine.logicalQueueFamilies.transferIndex
    };

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer
    (
        engine,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory,
        uniqueFamilyIndices
    );

    void* data;
    vkMapMemory(engine.logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    ::memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(engine.logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage
    (
        engine,
        texWidth,
        texHeight,
        engine.mipLevels,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        engine.textureImage,
        engine.textureImageMemory
    );

    transitionImageLayout
    (
        engine,
        engine.textureImage,
        engine.mipLevels,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    copyBufferToImage
    (
        engine,
        stagingBuffer,
        engine.textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight)
    );

    generateMipmaps
    (
        engine,
        engine.textureImage,
        engine.mipLevels,
        static_cast<int32_t>(texWidth),
        static_cast<int32_t>(texHeight)
    );

    vkDestroyBuffer(engine.logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(engine.logicalDevice, stagingBufferMemory, nullptr);

    engine.textureImageView =   createImageView
                                (
                                    engine,
                                    engine.textureImage,
                                    engine.mipLevels,
                                    VK_FORMAT_R8G8B8A8_SRGB,
                                    VK_IMAGE_ASPECT_COLOR_BIT
                                );
}

void createImage
(
    const EngineData            &engine,
    const uint32_t              &width,
    const uint32_t              &height,
    const uint32_t              &mipLevels,
    const VkFormat              &format,
    const VkImageTiling         &tiling,
    const VkImageUsageFlags     &usage,
    const VkMemoryPropertyFlags &memPropFlags,
    VkImage                     &image,
    VkDeviceMemory              &imageMem
){
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = mipLevels;
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
        vkCreateImage(engine.logicalDevice, &imageCreateInfo, nullptr, &image),
        "failed to create image!"
    );

    VkMemoryRequirements imageMemRequirements;
    vkGetImageMemoryRequirements(engine.logicalDevice, image, &imageMemRequirements);

    VkMemoryAllocateInfo mAllocInfo{};
    mAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mAllocInfo.allocationSize = imageMemRequirements.size;
    mAllocInfo.memoryTypeIndex = findSuitableMemoryType(engine, imageMemRequirements.memoryTypeBits, memPropFlags);

    riverAssertVkSuccess
    (
        vkAllocateMemory(engine.logicalDevice, &mAllocInfo, nullptr, &imageMem),
        "failed to allocate image memory!"
    );

    vkBindImageMemory(engine.logicalDevice, image, imageMem, 0);
}

VkImageView createImageView
(
    const EngineData            &engine,
    const VkImage               &image,
    const uint32_t              &mipLevels,
    const VkFormat              &format,
    const VkImageAspectFlags    &aspectFlags
){
    VkImageViewCreateInfo viewCreateInfo{};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image = image;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.format = format;
    viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = mipLevels;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    riverAssertVkSuccess
    (
        vkCreateImageView(engine.logicalDevice, &viewCreateInfo, nullptr, &imageView),
        "failed to create texture image view!"
    );

    return imageView;
}

void createTextureSampler
(
    EngineData &engine
){
    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy = engine.deviceProperties.limits.maxSamplerAnisotropy;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = static_cast<float>(engine.mipLevels);

    riverAssertVkSuccess
    (
        vkCreateSampler(engine.logicalDevice, &samplerCreateInfo, nullptr, &engine.textureSampler),
        "failed to create texture sampler!"
    );
}
