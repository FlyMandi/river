#pragma once

#include "vulkan/vulkan_core.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>

namespace engine{


const int MAX_FRAMES_IN_FLIGHT = 2;

extern std::filesystem::path appRoot;
extern std::filesystem::path debugLog;

extern VkInstance instance;

extern VkQueue graphicsQueue;
extern VkQueue presentQueue;


extern VkRenderPass renderPass;
extern VkPipelineLayout pipelineLayout;
extern VkPipeline graphicsPipeline;
extern VkCommandPool commandPool;
extern VkCommandBuffer commandBuffer;

extern VkSemaphore imageAvailableSemaphore;
extern VkSemaphore renderFinishedSemaphore;
extern VkFence inFlightFence;

extern void printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines);

extern void createInstance();
extern void setupDebugMessenger();
extern void createSurface();
extern void createGraphicsPipeline();
extern void createFramebuffers();
extern void createCommandPool();
extern void createCommandBuffer();
extern void createSyncObjects();
extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

};
