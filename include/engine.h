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

extern void createSurface();
extern void createInstance();

extern void createCommandBuffer();
extern void createCommandPool();
extern void createSyncObjects();
extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
extern void createFramebuffers();

extern void printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines);

extern void setupDebugMessenger();

};
