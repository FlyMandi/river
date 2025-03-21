#include "river.h"
#include "window.h"

#include <stdexcept>

void initGLFW(){
    glfwInit();

    //TODO: add window resizeability
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, appName, nullptr, nullptr);

    if(nullptr == window){
        printDebugLog('\0', "failed to create GLFW window!", '\n');
        throw std::runtime_error("failed to create GLFW window!");
    }
}

void cleanupGLFW(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void createSurface(){
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        printDebugLog('\n', "failed to create window surface!");
        throw std::runtime_error("failed to create window surface!");
    }
}
