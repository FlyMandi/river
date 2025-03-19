#pragma once

#include "debugger.h"
#include "vulkan/vulkan_core.h"

typedef struct Pipeline{

Pipeline();
~Pipeline();

VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;
VkPipeline graphicsPipeline;
VkCommandPool commandPool;
VkCommandBuffer commandBuffer;

VkSemaphore imageAvailableSemaphore;
VkSemaphore renderFinishedSemaphore;
VkFence inFlightFence;

void createGraphicsPipeline();

}Pipeline;
