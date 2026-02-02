#include "river.h"

#include "h/engine.h"

void river::initGLFW(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    engine::window = glfwCreateWindow(WIDTH, HEIGHT, ((std::string)appName + " " + (std::string)appVersion).c_str(), nullptr, nullptr);
}

void river::cleanupGLFW(){
    glfwDestroyWindow(engine::window);
    glfwTerminate();
}

void river::windowPollEvents(){
//TODO: better use of GLFW event system, there has to be a better way to check for window exit
    glfwPollEvents();
    window_SHOULDCLOSE = glfwWindowShouldClose(engine::window);
}
