#pragma once

#include "vulkan/vulkan_core.h"

extern void createImage
(
    uint32_t                width,
    uint32_t                height,
    VkFormat                format,
    VkImageTiling           tiling,
    VkImageUsageFlags       usage,
    VkMemoryPropertyFlags   memPropFlags,
    VkImage                 &image,
    VkDeviceMemory          &imageMem
);

VkImageView createImageView(VkImage image, VkFormat format);

extern void createTextureImage();
extern void createTextureImageView();

inline VkImage textureImage;
inline VkImageView textureImageView;
inline VkDeviceMemory textureImageMemory;

inline VkSampler textureImageSampler;
