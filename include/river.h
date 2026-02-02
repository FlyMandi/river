#pragma once

#include "vulkan/vulkan_core.h"

#include <filesystem>
#include <vector>
#include <fstream>

inline uint32_t currentFrame = 0;

constexpr auto ENGINE_NAME = "River";

constexpr uint8_t logLevel = 0;

inline const char* projectName;
inline std::string projectVersion;

inline std::filesystem::path projectRoot;
inline std::filesystem::path projectLog;
#define projectLogFolder = projectRoot / "log";

inline VkInstance instance;

extern void initVulkan();
extern void cleanupVulkan();

extern void drawFrame();

extern void getProjectRoot(const char *rootName);
extern void riverSetupLog();
extern void riverCloseLog();

inline std::ofstream logFile;

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
    RIV_LOG_LEVEL_UNDEFINED = 5
};

const char* riverTranslateVkResult(VkResult code);

extern void riverLog(const std::string_view text, const RiverLogLevel level);
extern void riverAssert(bool condition, const std::string_view assertFailureMsg);
extern void riverAssertVkSuccess(VkResult result, const std::string_view assertFailureMsg);
extern void riverThrow(const std::string_view throwMsg);
