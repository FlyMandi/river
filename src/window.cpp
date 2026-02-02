#include "river.h"
#include "window.h"
#include "pipeline.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    framebufferResized = VK_TRUE;
}

void initGLFW(GLFWwindow* window, int width, int height, const char* windowName)
{
    glfwInit();

    //TODO:#49: choose monitor, refresh rate, videoMode, etc
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window =    glfwCreateWindow
                (
                    width,
                    height,
                    windowName,
                    nullptr,
                    nullptr
                );

    riverAssert(nullptr != window, "failed to create GLFW window!");

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void cleanupGLFW(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void createSurface(GLFWwindow *window, VkSurfaceKHR &surface)
{
    //TODO:#39: find out if I can create a surface smaller than the window.
    //GLFW sub-windows? or Vulkan scissor?
    riverAssertVkSuccess
    (
        glfwCreateWindowSurface(instance, window, nullptr, &surface),
        "failed to create window surface!"
    );
}
