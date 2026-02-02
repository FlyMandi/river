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

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = static_cast<uint32_t>(texWidth);
    imageCreateInfo.extent.height = static_cast<uint32_t>(texHeight);
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    //FIXME:#50: WIP
}
