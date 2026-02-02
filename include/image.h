#pragma once

#include "vulkan/vulkan_core.h"

#include <filesystem>

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

VkImageView createImageView
(
    VkImage             image,
    VkFormat            format,
    VkImageAspectFlags  aspectFlags
);

extern void createTextureImage(const std::filesystem::path &texturePath);
extern void createTextureImageView();
extern void createTextureSampler();

extern void transitionImageLayout
(
    VkImage         image,
    VkFormat        format,
    VkImageLayout   oldLayout,
    VkImageLayout   newLayout
);

inline VkImage textureImage;
inline VkImageView textureImageView;
inline VkDeviceMemory textureImageMemory;

inline VkSampler textureSampler;
