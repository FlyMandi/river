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

static const char* logLevelStamps[] =
{
    "[RIV_TRACE]: ",
    "[RIV_DEBUG]: ",
    "[RIV_WARNI]: ",
    "[RIV_ERROR]: ",
};

static const char* logLevelANSI[] =
{
    "\033[30;1;1m",
    "\033[37;1;1m",
    "\033[33;1;1m",
    "\033[31;1;1m"
};

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

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cerr << '\n' << std::put_time(&buf, "[%T]-") << "[RIV_ASSERT]: " << result << ": " << assertFailureMsg << '\n';
    abort();
}
