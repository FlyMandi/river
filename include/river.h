#pragma once

#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

#include <filesystem>

#if defined(DEBUG) || defined(_DEBUG)
    const bool build_DEBUG = true;
#else
    const bool build_DEBUG = false;
#endif


typedef struct River{

River() = delete;
River(const char* appName, const char* appVersion) : appName(appName), appVersion(appVersion){};
River(const River&) = delete;
River operator=(const River &) = delete;
~River();

const int MAX_FRAMES_IN_FLIGHT = 2;

std::filesystem::path appRoot;
std::filesystem::path debugLog;

Window window;
Device device;
SwapChain swapchain = SwapChain(device);
Pipeline pipeline;

VkInstance instance;

//TODO: move queues & command buffers to diff struct
VkQueue graphicsQueue;
VkQueue presentQueue;

void createInstance();

void createCommandBuffer();
void createCommandPool();
void createSyncObjects();
void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
void createFramebuffers();

void setupDebugMessenger();

const char *engineName = "River";
const char *appName;
const char *appVersion;

void initVulkan();
void cleanupVulkan();

void drawFrame();

void getProjectRoot(const char* rootName);

}River;
