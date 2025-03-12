#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

extern GLFWwindow *window;

void run();
void initWindow();
void gameLoop();

void createMainMenu(); 
