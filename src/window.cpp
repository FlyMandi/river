#include "river.h"
#include "window.h"

#include <stdexcept>
#include <string>

void window::initGLFW(){
    glfwInit();

    //TODO: add window resizeability
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, ((std::string)river::appName + " " + (std::string)river::appVersion).c_str(), nullptr, nullptr);
    if(nullptr == pWindow){
        river::printDebugLog("failed to GLFW window!", 0, 2);
        throw std::runtime_error("failed to create GLFW window!");
    }else{
        river::printDebugLog("Successfully created GLFW window.", 0, 2);
    }
}

void window::cleanupGLFW(){
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

void window::pollEvents(){
    //TODO: there has to be a better way to check for window exit
    glfwPollEvents();
    SHOULDCLOSE = glfwWindowShouldClose(pWindow);
}

void window::createSurface(){
    if(glfwCreateWindowSurface(river::instance, pWindow, nullptr, &surface) != VK_SUCCESS){
        river::printDebugLog("failed to create window surface!", 0, 2);
        throw std::runtime_error("failed to create window surface!");
    }else{
        river::printDebugLog("Successfully created window surface.", 0, 2);
    }
}
