#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void run();
void initGLFW();
void gameLoop();
void GLFWcleanup();

void createMainMenu(); 
