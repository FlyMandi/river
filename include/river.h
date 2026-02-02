#pragma once

#include "vulkan/vulkan_core.h"

#include <filesystem>
#include <vector>
#include <chrono>
#include <iomanip>
#include <iostream>

#define TIMESTAMP 

inline uint8_t currentFrame = 0;

constexpr auto ENGINE_NAME = "River";

//if specified logLevel is 4+, only asserts will be thrown
constexpr uint8_t logLevel = 1;
 
inline const char *appName;
inline const char *appVersion;

inline std::filesystem::path appRoot;

inline VkInstance instance;
inline VkDebugUtilsMessengerEXT debugMessenger;

#ifdef DEBUG
const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_KHRONOS_synchronization2",
    // TODO:#47: re-enable when this is set to trace
    // "VK_LAYER_LUNARG_crash_diagnostic",
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

//ugly aah inline function definitions in header file, I know
//this is because of the auto types, they need to be known BEFORE the link stage

//TODO:#43: write logs & asserts to a file in release builds AND
//show an actual useful runtime error message box, not just "abort has been called"
inline void riverLog(const auto &text, const RiverLogLevel level)
{
    if(level < logLevel)
    {
        return;
    }

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    switch(level)
    {
        case RIV_LOG_LEVEL_TRACE:
            std::cout << std::put_time(&buf, "[%T]-") << "[RIV_TRACE]: " << text << '\n';
            return;

        case RIV_LOG_LEVEL_DEBUG:
            std::cout << std::put_time(&buf, "[%T]-") << "[RIV_DEBUG]: " << text << '\n';
            return;

        case RIV_LOG_LEVEL_WARN:
            std::cerr << std::put_time(&buf, "[%T]-") << "[RIV_WARN]:  " << text << '\n';
            return;

        case RIV_LOG_LEVEL_ERROR:
            std::cerr << std::put_time(&buf, "[%T]-") << "[RIV_ERROR]: " << text << '\n';
            return;

        case RIV_LOG_LEVEL_ASSERT:
            std::cerr << std::put_time(&buf, "[%T]-") << "[RIV_ASSERT]: " << text << '\n';
            abort();
    }
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
