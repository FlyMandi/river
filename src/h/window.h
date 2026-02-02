#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "utils.h"
#include <filesystem>

inline const char *appName = "Cnake";
inline const char *engineName = "CNNgine";
inline const char *version = "0.0.1";

inline std::filesystem::path appRoot = getProjectRoot(appName);

inline const uint32_t WIDTH = 1920;
inline const uint32_t HEIGHT = 1080;

inline bool window_SHOULDCLOSE = false;

inline GLFWwindow *window;

void initGLFW();
void cleanupGLFW();

void windowPollEvents();
