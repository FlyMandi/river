#pragma once

#include "vulkan/vulkan_core.h"

#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <vector>
#include <chrono>

#define TIMESTAMP 

inline uint16_t currentFrame = 0;

constexpr auto ENGINE_NAME = "River";
 
inline const char *appName;
inline const char *appVersion;

inline std::filesystem::path appRoot;

inline VkInstance instance;
inline VkDebugUtilsMessengerEXT debugMessenger;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

extern void initVulkan();
extern void cleanupVulkan();

extern void drawFrame();
extern void createDescriptorSetLayout();

extern std::filesystem::path getProjectRoot(const char *rootName);
extern void clearLogs(const std::filesystem::path &baseDir);

#ifdef DEBUG
//TODO:#37: eventually add log levels: [TRACE], [DEBUG], [WARN], [ERROR]
//pass as argument to function, [DEBUG] by default
//obv trace shows all, error shows least

void printDebugLog(const auto &text){
    std::cout << text;
}

void printDebugLog(const auto &text, const char &newline){
    std::cout << text << newline;
}

void printDebugLog(const char &tab, const auto &text){
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cout << std::put_time(&buf, "[%T] ") << tab << text;
}

void printDebugLog(const char &tab, const auto &text, const char &newline){
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    tm buf;
    localtime_s(&buf, &now);

    std::cout << std::put_time(&buf, "[%T] ") << tab << text << newline;
}
#endif
