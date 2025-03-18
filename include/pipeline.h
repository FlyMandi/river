#pragma once

#include "vulkan/vulkan_core.h"

namespace pipeline{

inline VkRenderPass renderPass;
inline VkPipelineLayout pipelineLayout;
inline VkPipeline graphicsPipeline;
inline VkCommandPool commandPool;
inline VkCommandBuffer commandBuffer;

inline VkSemaphore imageAvailableSemaphore;
inline VkSemaphore renderFinishedSemaphore;
inline VkFence inFlightFence;

extern void createGraphicsPipeline();

};
