#include "river.h"
#include "window.h"
#include "pipeline.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    riverLog("attempting resize: ", RIV_LOG_LEVEL_TRACE);
    riverLog(height, RIV_LOG_LEVEL_TRACE, false);
    riverLog("x", RIV_LOG_LEVEL_TRACE, false);
    riverLog(width, RIV_LOG_LEVEL_TRACE, false);

    framebufferResized = VK_TRUE;
}

void initGLFW()
{
    glfwInit();

    //TODO:#49: choose monitor, refresh rate, videoMode, etc
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, appName, nullptr, nullptr);

    riverAssert(nullptr != window, "failed to create GLFW window!");

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
    riverAssertVkSuccess
    (
        glfwCreateWindowSurface(instance, window, nullptr, &surface),
        "failed to create window surface!"
    );
}
