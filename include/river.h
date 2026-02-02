#pragma once

#include <cstdint>

namespace river{
    
inline bool window_SHOULDCLOSE = false;

inline const char *engineName = "River";
inline const char *appName;
inline const char *appVersion;

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

void initVulkan();
void cleanupVulkan();

void initGLFW();
void cleanupGLFW();

void drawFrame();
void windowPollEvents();

void getProjectRoot(const char* rootName);
}
