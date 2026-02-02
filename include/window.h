#pragma once

#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

extern void initGLFW
(
    GLFWwindow  *window,
    const char* projectName,
    int         width,
    int         height
);

extern void cleanupGLFW(GLFWwindow *window);

extern void createSurface
(
    GLFWwindow      *window,
    VkSurfaceKHR    &surface
);
