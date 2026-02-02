#pragma once

#include <vulkan/vulkan_core.h>

#include "river.h"

extern void createImage
(
    const EngineData            &engine,
    const uint32_t              &width,
    const uint32_t              &height,
    const VkFormat              &format,
    const VkImageTiling         &tiling,
    const VkImageUsageFlags     &usage,
    const VkMemoryPropertyFlags &memPropFlags,
    VkImage                     &image,
    VkDeviceMemory              &imageMem
);

extern VkImageView createImageView
(
    const EngineData    &engine,
    VkImage             image,
    VkFormat            format,
    VkImageAspectFlags  aspectFlags
);

extern void createTextureImage
(
    const EngineData        &engine,
    const ProjectManifest   &manifest
);

extern void createTextureImageView
(
    const EngineData &engine
);

extern void createTextureSampler
(
    const EngineData &engine
);

extern void transitionImageLayout
(
    const EngineData    &engine,
    const VkImage       &image,
    const VkFormat      &format,
    const VkImageLayout &oldLayout,
    const VkImageLayout &newLayout
);

//TODO:#57: rid of inline global scope vars
inline VkImage textureImage;
inline VkImageView textureImageView;
inline VkDeviceMemory textureImageMemory;

inline VkSampler textureSampler;
