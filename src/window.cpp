#include "window.h"
#include "pipeline.h"
#include "river.h"

#include <stdexcept>

static void framebufferResizeCallback(GLFWwindow* window, int width, int height){
    framebufferResized = true;
}

void initGLFW(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, appName, nullptr, nullptr);

    if(nullptr == window){
        printDebugLog('\0', "failed to create GLFW window!", '\n');
        throw std::runtime_error("failed to create GLFW window!");
    }

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void cleanupGLFW(){
    glfwDestroyWindow(window);
    glfwTerminate();
}


void createSurface(){
    //TODO: find out if I can create a surface smaller than the window.
    //GLFW sub-windows? or Vulkan scissor?
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        printDebugLog('\n', "failed to create window surface!");
        throw std::runtime_error("failed to create window surface!");
    }
}
