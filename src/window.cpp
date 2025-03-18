#include "engine.h"
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
        engine::printDebugLog("\nERROR: failed to create GLFW window!", 0, 1);
        throw std::runtime_error("failed to create GLFW window!");
    }
}

void window::cleanupGLFW(){
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}

void window::pollEvents(){
//TODO: better use of GLFW event system, there has to be a better way to check for window exit
    glfwPollEvents();
    SHOULDCLOSE = glfwWindowShouldClose(pWindow);
}
