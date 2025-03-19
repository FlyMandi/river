#include "window.h"

#include <stdexcept>
#include <string>

void Window::initGLFW(){
    glfwInit();

    //TODO: add window resizeability
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, ((std::string)windowName + " " + (std::string)windowVersion).c_str(), nullptr, nullptr);
    if(nullptr == pWindow){
        throw std::runtime_error("failed to create GLFW window!");
    }
}

void Window::cleanupGLFW(){
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

void Window::pollEvents(){
    //TODO: there has to be a better way to check for window exit
    glfwPollEvents();
    SHOULDCLOSE = glfwWindowShouldClose(pWindow);
}

void Window::createSurface(){
    if(glfwCreateWindowSurface(instance, pWindow, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("failed to create window surface!");
    }
}
