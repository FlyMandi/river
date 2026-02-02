#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>

inline VkRenderPass renderPass;
inline VkPipeline graphicsPipeline;
inline VkPipelineLayout graphicsPipelineLayout;

inline VkDescriptorSetLayout descriptorSetLayout;
inline std::vector<VkDescriptorSet> descriptorSets;

inline VkCommandPool graphicsCommandPool;
inline VkCommandPool transferCommandPool;
inline VkDescriptorPool descriptorPool;

inline std::vector<VkCommandBuffer> commandBuffers;

inline VkQueue graphicsQueue;
inline VkQueue presentQueue;
inline VkQueue transferQueue;

inline std::vector<VkSemaphore> imageAvailableSemaphores;
inline std::vector<VkSemaphore> renderFinishedSemaphores;
inline std::vector<VkFence> inFlightFences;

inline VkBool32 framebufferResized = VK_FALSE;

extern void createGraphicsPipeline();
extern void createFramebuffers();

extern void createCommandPools();
extern void createCommandBuffers();

extern VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool);
extern void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue);

extern void createDescriptorSetLayout();
extern void createDescriptorPool();
extern void createDescriptorSets();

extern void createSyncObjects();

extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
