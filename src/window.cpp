#include "river.h"
#include "window.h"

#include <stdexcept>
#include <string>

void initGLFW(){
    glfwInit();

    //TODO: add window resizeability
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, ((std::string)appName + " " + (std::string)appVersion).c_str(), nullptr, nullptr);

    if(nullptr == window){
        printDebugLog("failed to GLFW window!", 0, 2);
        throw std::runtime_error("failed to create GLFW window!");
    }else{
        printDebugLog("Successfully created GLFW window.", 0, 2);
    }
}

void cleanupGLFW(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void createSurface(){
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        printDebugLog("failed to create window surface!", 0, 2);
        throw std::runtime_error("failed to create window surface!");
    }else{
        printDebugLog("Successfully created window surface.", 0, 2);
    }
}
