#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

inline GLFWwindow *window;

inline uint32_t WIDTH;
inline uint32_t HEIGHT;

inline VkSurfaceKHR surface;

extern void initGLFW();
extern void cleanupGLFW();

extern void pollEvents();
extern void createSurface();
