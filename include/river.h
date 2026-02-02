#pragma once

#include "vulkan/vulkan_core.h"

#include <filesystem>
#include <vector>

#define TIMESTAMP 

inline uint8_t currentFrame = 0;

constexpr auto ENGINE_NAME = "River";
constexpr uint8_t logLevel = 1;
 
inline const char *appName;
inline const char *appVersion;

inline std::filesystem::path appRoot;

inline VkInstance instance;
inline VkDebugUtilsMessengerEXT debugMessenger;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

extern void initVulkan();
extern void cleanupVulkan();

extern void drawFrame();

extern std::filesystem::path getProjectRoot(const char *rootName);
extern void clearLogs(const std::filesystem::path &baseDir);

//if specified logLevel is 4+, only asserts will be thrown
enum RiverLogLevel
{
    RIV_LOG_LEVEL_TRACE  = 0,
    RIV_LOG_LEVEL_DEBUG  = 1,
    RIV_LOG_LEVEL_WARN   = 2,
    RIV_LOG_LEVEL_ERROR  = 3,
    RIV_LOG_LEVEL_ASSERT = 4
};

extern void riverLog(const auto &text, const RiverLogLevel level);

extern void riverAssert(bool condition, const auto &assertFailureMsg);
extern void riverAssertVkSuccess(VkResult result, const auto &assertFailureMsg);
