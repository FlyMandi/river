#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace window{

inline bool SHOULDCLOSE = false;

inline uint32_t WIDTH;
inline uint32_t HEIGHT;

inline GLFWwindow *pWindow;

inline VkSurfaceKHR surface;

extern void initGLFW();
extern void cleanupGLFW();
extern void pollEvents();

extern void createSurface();

}
