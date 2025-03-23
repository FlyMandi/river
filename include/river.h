#pragma once

#include "vulkan/vulkan_core.h"

//OPTIM: try to minimize the amount of data types. See if it makes sense to make own data structures
//vectors, for example, since I'm really not using most (if any) of the functionality
//maybe just get rid of vectors in favour of another resizeable list
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <vector>
#include <chrono>

#define TIMESTAMP 

#ifdef DEBUG
    #define BUILD_DEBUG true 
#else
    #define BUILD_DEBUG false 
#endif

#define MAX_FRAMES_IN_FLIGHT 2
#define ENGINE_NAME "River"
//TODO: rename this, because when debugging, there is no log (lmao)
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

extern std::filesystem::path getProjectRoot(const char *rootName);
extern void clearLogs(const std::filesystem::path &baseDir);

#ifdef DEBUG

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

#else

void printDebugLog(const auto& text){ return; }

void printDebugLog(const char &tab, const auto &text){ return; }

void printDebugLog(const auto &text, const char &newline){ return; }

void printDebugLog(const char &tab, const auto &text, const char &newline){ return; }

#endif
