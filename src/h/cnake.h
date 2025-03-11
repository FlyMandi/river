#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class CnakeApp{
    const char *version = "Cnake 0.0.0";
    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

    //TODO: make sure this points to the same exact window as Engine::window
    GLFWwindow *window;

    //TODO: remove copy constructor & operator
    //deal with constructors & destructors, default constructors

    void initWindow();
    void gameLoop();
 public:
    void run();
};
