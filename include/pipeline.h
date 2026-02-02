#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>

inline VkRenderPass renderPass;
inline VkPipeline graphicsPipeline;
inline VkPipelineLayout pipelineLayout;

inline VkDescriptorSetLayout descriptorSetLayout;

inline VkCommandPool graphicsCommandPool;
inline VkCommandPool transferCommandPool;
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
extern void createSyncObjects();

extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
