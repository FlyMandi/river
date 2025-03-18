#pragma once

#include "vulkan/vulkan_core.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>

namespace engine{


const int MAX_FRAMES_IN_FLIGHT = 2;

inline std::filesystem::path appRoot;
inline std::filesystem::path debugLog;

inline VkInstance instance;

inline VkQueue graphicsQueue;
inline VkQueue presentQueue;

inline VkRenderPass renderPass;
inline VkPipelineLayout pipelineLayout;
inline VkPipeline graphicsPipeline;
inline VkCommandPool commandPool;
inline VkCommandBuffer commandBuffer;

inline VkSemaphore imageAvailableSemaphore;
inline VkSemaphore renderFinishedSemaphore;
inline VkFence inFlightFence;

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
