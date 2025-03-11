#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>

class CnakeApp{
    const char *version = "Cnake 0.0.0";
    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

    GLFWwindow *window;

    //TODO: remove copy constructor & operator
    //add default constructor that binds CnakeApp to Engine

    void initWindow();
    void gameLoop();
 public:
    void run();
};
