#pragma once

#include "vulkan/vulkan_core.h"

#include <filesystem>
#include <vector>

#if defined(DEBUG) || defined(_DEBUG)
    const bool build_DEBUG = true;
#else
    const bool build_DEBUG = false;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

namespace river{

inline const char *engineName = "River";
inline const char *appName;
inline const char *appVersion;

inline std::filesystem::path appRoot;
inline std::filesystem::path debugLog;

inline VkInstance instance;
inline VkDebugUtilsMessengerEXT debugMessenger;

//TODO: move queues & command buffers to diff file 

extern void initVulkan();
extern void cleanupVulkan();

extern void drawFrame();

extern void getProjectRoot(const char* rootName);
extern void printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines);

inline const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation" 
};

inline bool firstOpen = true;

}
