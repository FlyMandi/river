#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

inline GLFWwindow *window;

inline uint32_t riverWindowWidth;
inline uint32_t riverWindowHeight;

inline const char* riverModelPath = "RIV_UNINITIALIZED_MODEL_PATH";
inline const char* riverTexturePath = "RIV_UNINITIALIZED_TEXTURE_PATH";

inline VkSurfaceKHR surface;

extern void initGLFW();
extern void cleanupGLFW();

extern void createSurface();
