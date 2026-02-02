#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>

inline VkRenderPass renderPass;
inline VkPipelineLayout pipelineLayout;
inline VkPipeline graphicsPipeline;
inline VkCommandPool commandPool;
inline std::vector<VkCommandBuffer> commandBuffers;

inline VkQueue graphicsQueue;
inline VkQueue presentQueue;
inline std::vector<VkSemaphore> imageAvailableSemaphores;
inline std::vector<VkSemaphore> renderFinishedSemaphores;
inline std::vector<VkFence> inFlightFences;

inline bool framebufferResized = false;

extern void createGraphicsPipeline();
extern void createFramebuffers();
extern void createCommandPool();
extern void createCommandBuffers();
extern void createSyncObjects();

extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
