#pragma once

#include "vulkan/vulkan_core.h"

namespace pipeline{

inline VkRenderPass renderPass;
inline VkPipelineLayout pipelineLayout;
inline VkPipeline graphicsPipeline;
inline VkCommandPool commandPool;
inline VkCommandBuffer commandBuffer;

inline VkQueue graphicsQueue;
inline VkQueue presentQueue;

inline VkSemaphore imageAvailableSemaphore;
inline VkSemaphore renderFinishedSemaphore;
inline VkFence inFlightFence;

extern void createGraphicsPipeline();
extern void createFramebuffers();
extern void createCommandPool();
extern void createCommandBuffer();
extern void createSyncObjects();

extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

}
