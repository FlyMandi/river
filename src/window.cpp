#include "h/window.h"

void initGLFW(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, ((std::string)appName + " " + (std::string)version).c_str(), nullptr, nullptr);
}

void cleanupGLFW(){
    glfwDestroyWindow(window);
    glfwTerminate();
}
