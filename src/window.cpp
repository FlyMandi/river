#include "river.h"

void initGLFW(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, ((std::string)appName + " " + (std::string)appVersion).c_str(), nullptr, nullptr);
}

void cleanupGLFW(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void windowPollEvents(){
    glfwPollEvents();
    window_SHOULDCLOSE = glfwWindowShouldClose(window);
}
