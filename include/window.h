#pragma once

#include "GLFW/glfw3.h"

extern void initGLFW(GLFWwindow* window, int width, int height, const char* projectName);
extern void cleanupGLFW();

extern void createSurface();
