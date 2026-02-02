#pragma once

#include "vulkan/vulkan_core.h"

#include <filesystem>
#include <vector>
#include <chrono>
#include <iomanip>
#include <iostream>

#define TIMESTAMP

inline uint32_t currentFrame = 0;

constexpr auto ENGINE_NAME = "River";

//if specified logLevel is 4+, only asserts will be thrown
constexpr uint8_t logLevel = 0;

inline const char *riverAppName;
inline const char *riverAppVersion;

inline std::filesystem::path riverAppRoot;

inline VkInstance instance;
inline VkDebugUtilsMessengerEXT debugMessenger;

#ifdef DEBUG
const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_KHRONOS_synchronization2",
    // "VK_LAYER_LUNARG_crash_diagnostic",
    "VK_LAYER_LUNARG_monitor",
    "VK_LAYER_RTSS"
};
#endif

extern void initVulkan();
extern void cleanupVulkan();

extern void drawFrame();

extern std::filesystem::path getProjectRoot(const char *rootName);
extern void clearLogs(const std::filesystem::path &baseDir);

enum RiverLogLevel
{
    RIV_LOG_LEVEL_TRACE  = 0,
    RIV_LOG_LEVEL_DEBUG  = 1,
    RIV_LOG_LEVEL_WARN   = 2,
    RIV_LOG_LEVEL_ERROR  = 3,
    RIV_LOG_LEVEL_ASSERT = 4
};

static inline const char* logLevelStamps[] =
{
    "[RIV_TRACE]: ",
    "[RIV_DEBUG]: ",
    "[RIV_WARNI]: ",
    "[RIV_ERROR]: ",
    "[RIV_ASSER]: "
};

static inline const char* logLevelANSI[] =
{
    "\033[30;1;1m",
    "\033[37;1;1m",
    "\033[33;1;1m",
    "\033[31;1;1m",
    "\033[31;1;7m"
};

static inline const char* translateVkResult(VkResult code)
{
    switch(code)
    {
        case 0:             return "VK_SUCCESS";
        case 1:             return "VK_NOT_READY";
        case 2:             return "VK_TIMEOUT";
        case 3:             return "VK_EVENT_SET";
        case 4:             return "VK_EVENT_RESET";
        case 5:             return "VK_INCOMPLETE";
        case -1:            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case -2:            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case -3:            return "VK_ERROR_INITIALIZATION_FAILED";
        case -4:            return "VK_ERROR_DEVICE_LOST";
        case -5:            return "VK_ERROR_MEMORY_MAP_FAILED";
        case -6:            return "VK_ERROR_LAYER_NOT_PRESENT";
        case -7:            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case -8:            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case -9:            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case -10:           return "VK_ERROR_TOO_MANY_OBJECTS";
        case -11:           return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case -12:           return "VK_ERROR_FRAGMENTED_POOL";
        case -13:           return "VK_ERROR_UNKNOWN";
        case -1000069000:   return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case -1000072003:   return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case -1000161000:   return "VK_ERROR_FRAGMENTATION";
        case -1000257000:   return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case 1000297000:    return "VK_PIPELINE_COMPILE_REQUIRED";
        case -1000174001:   return "VK_ERROR_NOT_PERMITTED";
        case -1000000000:   return "VK_ERROR_SURFACE_LOST_KHR";
        case -1000000001:   return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case 1000001003:    return "VK_SUBOPTIMAL_KHR";
        case -1000001004:   return "VK_ERROR_OUT_OF_DATE_KHR";
        case -1000003001:   return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case -1000011001:   return "VK_ERROR_VALIDATION_FAILED_EXT";
        case -1000012000:   return "VK_ERROR_INVALID_SHADER_NV";
        case -1000023000:   return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        case -1000023001:   return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        case -1000023002:   return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        case -1000023003:   return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        case -1000023004:   return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        case -1000023005:   return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        case -1000158000:   return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case -1000255000:   return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case 1000268000:    return "VK_THREAD_IDLE_KHR";
        case 1000268001:    return "VK_THREAD_DONE_KHR";
        case 1000268002:    return "VK_OPERATION_DEFERRED_KHR";
        case 1000268003:    return "VK_OPERATION_NOT_DEFERRED_KHR";
        case -1000299000:   return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        case -1000338000:   return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        case 1000482000:    return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
        case 1000483000:    return "VK_PIPELINE_BINARY_MISSING_KHR";
        case -1000483000:   return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
        default:            return "RIV_ERROR_NOT_TRANSLATED";
    }
}

//ugly aah inline function definitions in header file, I know
//this is because of the auto types, they need to be known BEFORE the link stage

//TODO:#43: write logs & asserts to a file in release builds AND
//show an actual useful runtime error message box, not just "abort has been called"
inline void riverLog
(
    const auto &text,
    const uint8_t &level,
    const bool newLine = true
){
    if(level < logLevel)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cout << logLevelANSI[level];

    if(newLine)
    {
        std::cout << '\n' << std::put_time(&buf, "[%T]-") << logLevelStamps[level];
    }

    if(level == RIV_LOG_LEVEL_WARN || level == RIV_LOG_LEVEL_ERROR)
    {
        std::cerr << text << "\033[0m";
        return;
    }

    std::cout << text << "\033[0m";
}

inline void riverAssert(bool condition, const auto &assertFailureMsg)
{
    if(condition)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << std::put_time(&buf, "[%T]-") << "[RIV_ASSERT]: " << assertFailureMsg << '\n';
    abort();
}

inline void riverAssertVkSuccess(VkResult result, const auto &assertFailureMsg)
{
    if(result == VK_SUCCESS)
    {
        return;
    }

    uint32_t logLevel = RIV_LOG_LEVEL_ASSERT;

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << logLevelANSI[logLevel] << std::put_time(&buf, "[%T]-") << logLevelStamps[logLevel]
        << translateVkResult(result) << ": " << assertFailureMsg << "\033[0m\n";

    abort();
}
