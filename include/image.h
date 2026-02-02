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

extern void createTextureImage();

inline VkImage textureImage;
inline VkDeviceMemory textureImageMemory;
