#pragma once

#include "vulkan/vulkan_core.h"

#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <vector>
#include <chrono>

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

enum RiverLogLevel
{
    RIV_LOG_LEVEL_TRACE = 0,
    RIV_LOG_LEVEL_DEBUG = 1,
    RIV_LOG_LEVEL_WARN  = 2,
    RIV_LOG_LEVEL_ERROR = 3
};

void riverLog(const auto &text, const RiverLogLevel level)
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
        std::cout << std::put_time(&buf, "[%T]-") << "[RIV_TRACE]: " << text << std::endl;
        return;

    case RIV_LOG_LEVEL_DEBUG:
        std::cout << std::put_time(&buf, "[%T]-") << "[RIV_DEBUG]: " << text << std::endl;
        return;

    case RIV_LOG_LEVEL_WARN:
        std::cout << std::put_time(&buf, "[%T]-") << "[RIV_WARN]:  " << text << std::endl;
        return;

    case RIV_LOG_LEVEL_ERROR:
        std::cerr << std::put_time(&buf, "[%T]-") << "[RIV_ERROR]: " << text << std::endl;
        abort();
    }
}
