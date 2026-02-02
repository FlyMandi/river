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

extern void createGraphicsPipeline(const ProjectManifest &manifest);
extern void createFramebuffers();

extern void createCommandPools();
extern void createCommandBuffers();

extern VkCommandBuffer setupCommandBuffer(VkCommandPool commandPool);
extern void flushCommandBuffer
(
    VkCommandBuffer commandBuffer,
    VkCommandPool   commandPool,
    VkQueue         queue
);

extern void createDescriptorSetLayout();
extern void createDescriptorPool();
extern void createDescriptorSets();

extern void createSyncObjects();
extern void cleanupSyncObjects();

extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
