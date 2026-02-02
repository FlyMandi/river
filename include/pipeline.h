#pragma once

#include "river.h"
#include "swapchain.h"
#include "vulkan/vulkan_core.h"

inline VkRenderPass renderPass;
inline VkPipeline graphicsPipeline;
inline VkPipelineLayout graphicsPipelineLayout;

inline VkDescriptorSetLayout descriptorSetLayout;
inline std::vector<VkDescriptorSet> descriptorSets{};

inline VkCommandPool graphicsCommandPool;
inline VkCommandPool transferCommandPool;
inline VkDescriptorPool descriptorPool;

inline std::vector<VkCommandBuffer> commandBuffers{};

inline VkQueue graphicsQueue;
inline VkQueue presentQueue;
inline VkQueue transferQueue;

inline std::vector<VkSemaphore> imageReadyForWriteSemaphores{VK_NULL_HANDLE};
inline std::vector<VkSemaphore> imageReadyForPresentSemaphores{VK_NULL_HANDLE};
inline VkSemaphore acquireSemaphore = VK_NULL_HANDLE;

inline std::vector<VkFence> inFlightFences(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);

inline VkBool32 framebufferResized = VK_FALSE;

extern void createGraphicsPipeline
(
    const EngineData        &engine,
    const ProjectManifest   &manifest
);

extern void createFramebuffers
(
    EngineData &engine
);

extern void createCommandPools
(
    const EngineData &engine
);

extern void createCommandBuffers
(
    const EngineData &engine
);

extern VkCommandBuffer setupCommandBuffer
(
    const EngineData    &engine,
    VkCommandPool       &commandPool //TODO:#57 redundant by the end
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
    const EngineData &engine
);

extern void createDescriptorPool
(
    const EngineData &engine
);

extern void createDescriptorSets
(
    const EngineData &engine
);

extern void createSyncObjects
(
    const EngineData &engine
);

extern void cleanupSyncObjects
(
    const EngineData &engine
);

extern void recordCommandBuffer
(
    EngineData      &engine,
    VkCommandBuffer &commandBuffer,
    uint32_t        &imageIndex
);
