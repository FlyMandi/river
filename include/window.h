#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace window{

inline bool window_SHOULDCLOSE = false;

extern const uint32_t WIDTH;
extern const uint32_t HEIGHT;

extern GLFWwindow *pWindow;

extern void initGLFW();
extern void cleanupGLFW();
extern void pollEvents();

};
