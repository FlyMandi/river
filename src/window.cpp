#include "river.h"
#include "window.h"

#include <string>

void window::initGLFW(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, ((std::string)river::appName + " " + (std::string)river::appVersion).c_str(), nullptr, nullptr);
}

void window::cleanupGLFW(){
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

void window::pollEvents(){
//TODO: better use of GLFW event system, there has to be a better way to check for window exit
    glfwPollEvents();
    window_SHOULDCLOSE = glfwWindowShouldClose(pWindow);
}
