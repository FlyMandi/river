#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

//TODO: remove copy constructors, etc
//TODO: add useful constructor w/ width, height, etc
typedef struct Window{

Window();
Window(const Window&) = delete;
Window operator=(const Window&) = delete;
~Window();

bool SHOULDCLOSE = false;

uint32_t WIDTH;
uint32_t HEIGHT;

GLFWwindow *pWindow;

VkSurfaceKHR surface;

void initGLFW();
void cleanupGLFW();
void pollEvents();

void createSurface();

}Window;
