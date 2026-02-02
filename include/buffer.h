#pragma once

#include <vulkan/vulkan_core.h>

#include "river.h"

#include <set>
#include <vector>
#include <array>
#include <filesystem>

extern void loadModel
(
    EngineData                  &engine,
    const std::filesystem::path &modelPath
);

extern void createBuffer
(
    const EngineData            &engine,
    const VkDeviceSize          &bufferSize,
    const VkBufferUsageFlags    &usageFlags,
    const VkMemoryPropertyFlags &memPropFlags,
    VkBuffer                    &buffer,
    VkDeviceMemory              &bufferMemory,
    const std::set<uint32_t>    &uniqueQueueFamilies
);

extern void createDepthResources
(
    EngineData &engine
);

extern uint32_t findSuitableMemoryType
(
    const EngineData            &engine,
    const uint32_t              &typeFilter,
    const VkMemoryPropertyFlags &flags
);

extern void createVertexBuffer
(
    EngineData &engine
);

extern void createUniformBuffers
(
    EngineData &engine
);

extern VkFormat findSupportedFormat
(
    const EngineData            &engine,
    const std::vector<VkFormat> &candidates,
    const VkImageTiling         &tiling,
    const VkFormatFeatureFlags  &features
);

extern void updateUniformBuffer
(
    const EngineData    &engine,
    uint32_t            currentImage //redundant?
);

extern VkVertexInputBindingDescription getVertexBindingDescription();
extern std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions();
