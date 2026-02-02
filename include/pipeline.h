#pragma once

#include "river.h"
#include "vulkan/vulkan_core.h"

extern void createGraphicsPipeline
(
    EngineData              &engine,
    const ProjectManifest   &manifest
);

extern void createFramebuffers
(
    EngineData &engine
);

extern void createCommandPools
(
    EngineData &engine
);

extern void createCommandBuffers
(
    EngineData &engine
);

extern VkCommandBuffer setupCommandBuffer
(
    const EngineData    &engine,
    VkCommandPool       &commandPool
);

extern void flushCommandBuffer
(
    const EngineData        &engine,
    const VkCommandBuffer   &commandBuffer,
    const VkCommandPool     &commandPool,
    const VkQueue           &queue
);

extern void createDescriptorSetLayout
(
    EngineData &engine
);

extern void createDescriptorPool
(
    EngineData &engine
);

extern void createDescriptorSets
(
    EngineData &engine
);

extern void createSyncObjects
(
    EngineData &engine
);

extern void cleanupSyncObjects
(
    EngineData &engine
);

extern void recordCommandBuffer
(
    const EngineData        &engine,
    const VkCommandBuffer   &commandBuffer,
    const uint32_t          &imageIndex
);
