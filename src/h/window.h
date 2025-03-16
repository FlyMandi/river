#pragma once

#include "GLFW/glfw3.h"

#include "utils.h"

#include <filesystem>

inline const char *appName = "Cnake";
inline const char *version = "0.0.1";

inline const uint32_t WIDTH = 1920;
inline const uint32_t HEIGHT = 1080;

inline GLFWwindow *window;

inline bool window_SHOULDCLOSE = false;

inline std::filesystem::path appRoot = getProjectRoot(appName);

void initGLFW();
void cleanupGLFW();

void windowPollEvents();
