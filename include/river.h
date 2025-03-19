#pragma once

#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "debugger.h"

#include <filesystem>

#if defined(DEBUG) || defined(_DEBUG)
    const bool build_DEBUG = true;
#else
    const bool build_DEBUG = false;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

typedef struct River{

River() = delete;
River(const char *appName, const char* appVersion) : appName(appName), appVersion(appVersion){};
River(const River&) = delete;
River operator=(const River &) = delete;
~River();

const char *engineName = "River";
const char *appName;
const char *appVersion;

std::filesystem::path appRoot;

VkInstance instance;

Window window = Window(instance, appName, appVersion);
Device device = Device(window);
Debugger debugger = Debugger(instance, device);
SwapChain swapchain = SwapChain(device);
Pipeline pipeline;

//TODO: move queues & command buffers to diff struct
VkQueue graphicsQueue;
VkQueue presentQueue;

void createInstance();

void createCommandBuffer();
void createCommandPool();
void createSyncObjects();
void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
void createFramebuffers();

void initVulkan();
void cleanupVulkan();

void drawFrame();

void getProjectRoot(const char* rootName);

}River;
