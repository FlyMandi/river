#pragma once

#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

//TODO: remove copy constructors, etc
//TODO: add useful constructor w/ width, height, etc
typedef struct Window{

Window();
Window(VkInstance &instance, const char* appName, const char* appVersion) : instance(instance), windowName(appName), windowVersion(appVersion){};
Window(const Window&) = delete;
Window operator=(const Window&) = delete;
~Window();

bool SHOULDCLOSE = false;

uint32_t WIDTH;
uint32_t HEIGHT;

const char *windowName;
const char *windowVersion;

VkInstance &instance;

GLFWwindow *pWindow;

VkSurfaceKHR surface;

void initGLFW();
void cleanupGLFW();
void pollEvents();

void createSurface();

}Window;
