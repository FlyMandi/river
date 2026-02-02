#pragma once

#include <vulkan/vulkan_core.h>

#include "river.h"

extern void createImage
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
);

extern VkImageView createImageView
(
    const EngineData            &engine,
    const VkImage               &image,
    const uint32_t              &mipLevels,
    const VkFormat              &format,
    const VkImageAspectFlags    &aspectFlags
);

extern void createTextureImage
(
    EngineData              &engine,
    const ProjectManifest   &manifest
);

extern void createTextureSampler
(
    EngineData &engine
);

extern void transitionImageLayout
(
    EngineData          &engine,
    const VkImage       &image,
    const uint32_t      &mipLevels,
    const VkFormat      &format,
    const VkImageLayout &oldLayout,
    const VkImageLayout &newLayout
);
