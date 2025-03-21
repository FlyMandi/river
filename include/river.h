#pragma once

#include "vulkan/vulkan_core.h"

//TODO: try to minimize the amount of data types. See if it makes sense to make own data structures
//vectors, for example, since I'm really not using most (if any) of the functionality
//maybe just get rid of vectors in favour of another resizeable list
#include <filesystem>
#include <vector>

#if defined(DEBUG) || defined(_DEBUG)
    #define BUILD_DEBUG true 
#else
    #define BUILD_DEBUG false 
#endif

#define MAX_FRAMES_IN_FLIGHT 2
#define ENGINE_NAME "River"
#define DEBUG_LOG appRoot / "debug.log" 
 
inline const char *appName;
inline const char *appVersion;

inline std::filesystem::path appRoot;

inline VkInstance instance;
inline VkDebugUtilsMessengerEXT debugMessenger;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

extern void initVulkan();
extern void cleanupVulkan();

extern void drawFrame();

extern std::filesystem::path getProjectRoot(const char* rootName);
extern void clearLogs(const std::filesystem::path &baseDir);
extern void printDebugLog(const std::string &text, uint32_t tabs, uint32_t newlines);
