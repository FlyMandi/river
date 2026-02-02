#include "vulkan/vulkan_core.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "river.h"
#include "image.h"
#include "buffer.h"
#include "device.h"

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
    std::memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    //FIXME:#50: WIP
}

void createImage
(
    uint32_t            &width,
    uint32_t            &height,
    VkFormat            &format,
    VkImageTiling       &tiling,
    VkImageUsageFlags   &usage,
    VkImage             &image,
    VkDeviceMemory      &imageMem
){
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;

    //NOTE: in an edge case, this format might not be supported. conversions will be done eventually
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.flags = 0;

    riverAssertVkSuccess
    (
        vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &textureImage),
        "failed to create texture image!"
    );

    VkMemoryRequirements imageMemRequirements;
    vkGetImageMemoryRequirements(logicalDevice, textureImage, &imageMemRequirements);

    VkMemoryAllocateInfo mAllocInfo{};
    mAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mAllocInfo.allocationSize = imageMemRequirements.size;
    mAllocInfo.memoryTypeIndex = findSuitableMemoryType(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    riverAssertVkSuccess
    (
        vkAllocateMemory(logicalDevice, &mAllocInfo, nullptr, &textureImageMemory),
        "failed to allocate texture image memory!"
    );

    vkBindImageMemory(logicalDevice, textureImage, textureImageMemory, 0);
}
