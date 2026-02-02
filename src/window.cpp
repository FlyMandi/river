#include "window.h"
#include "pipeline.h"
#include "river.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    riverLog("new swap height:", RIV_LOG_LEVEL_TRACE);
    riverLog(height, RIV_LOG_LEVEL_TRACE);
    riverLog("new swap width:", RIV_LOG_LEVEL_TRACE);
    riverLog(width, RIV_LOG_LEVEL_TRACE);
    framebufferResized = VK_TRUE;
}

void initGLFW()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, appName, nullptr, nullptr);

    if(nullptr == window)
    {
        riverLog("failed to create GLFW window!", RIV_LOG_LEVEL_ERROR);
    }

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void cleanupGLFW()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}


void createSurface()
{
    //TODO:#39: find out if I can create a surface smaller than the window.
    //GLFW sub-windows? or Vulkan scissor?
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        riverLog("failed to create window surface!", RIV_LOG_LEVEL_ERROR);
    }
}
