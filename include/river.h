#pragma once

#include "vulkan/vulkan_core.h"

#include <filesystem>
#include <vector>

#define TIMESTAMP

inline uint32_t currentFrame = 0;

constexpr auto ENGINE_NAME = "River";

constexpr uint8_t logLevel = 0;

inline const char *riverAppName;
inline const char *riverAppVersion;

inline std::filesystem::path riverAppRoot;

inline VkInstance instance;

extern void initVulkan();
extern void cleanupVulkan();

extern void drawFrame();

extern std::filesystem::path getProjectRoot(const char *rootName);
extern void clearLogs(const std::filesystem::path &baseDir);

#ifdef DEBUG
inline VkDebugUtilsMessengerEXT debugMessenger;

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_KHRONOS_synchronization2",
    // "VK_LAYER_LUNARG_crash_diagnostic",
    "VK_LAYER_LUNARG_monitor",
    "VK_LAYER_RTSS"
};
#endif

enum RiverLogLevel
{
    RIV_LOG_LEVEL_TRACE     = 0,
    RIV_LOG_LEVEL_DEBUG     = 1,
    RIV_LOG_LEVEL_WARN      = 2,
    RIV_LOG_LEVEL_ERROR     = 3,
    RIV_LOG_LEVEL_ASSERT    = 4,
    RIV_LOG_LEVEL_UNDEFINED = -4711
};

const char* riverTranslateVkResult(VkResult code);

//TODO:#43: write logs & asserts to a file in release builds AND
//show an actual useful runtime error message box, not just "abort has been called"
extern void riverLog(const char* text, const RiverLogLevel level);
extern void riverAssert(bool condition, const char* assertFailureMsg);
extern void riverAssertVkSuccess(VkResult result, const char* assertFailureMsg);
extern void riverThrow(const char* throwMsg);
