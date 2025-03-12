#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct CnakeApp{
    const char *version = "Cnake 0.0.0";
    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

    GLFWwindow *window;

    CnakeApp() = default;
    CnakeApp(const CnakeApp&) = delete;
    CnakeApp &operator=(const CnakeApp&) = delete;

    void run();
    void initWindow();
    void gameLoop();

    void createMainMenu(); 
};
