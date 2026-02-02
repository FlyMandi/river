#include "river.h"
#include "window.h"
#include "pipeline.h"

internal void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    framebufferResized = VK_TRUE;
}

void initGLFW
(
    EngineData              &engine,
    const UserSettings      &settings
){
    glfwInit();

    //TODO:#49: choose monitor, refresh rate, videoMode, etc
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    engine.window = glfwCreateWindow
                    (
                        settings.windowWidth,
                        settings.windowHeight,
                        engine.windowName.c_str(),
                        nullptr,
                        nullptr
                    );

    riverAssert(nullptr != engine.window, "failed to create GLFW window!");

    glfwSetFramebufferSizeCallback(engine.window, framebufferResizeCallback);
}

void cleanupGLFW(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void createSurface(EngineData &engine)
{
    //TODO:#39: find out if I can create a surface smaller than the window.
    //GLFW sub-windows? or Vulkan scissor?
    riverAssertVkSuccess
    (
        glfwCreateWindowSurface(engine.instance, engine.window, nullptr, &engine.surface),
        "failed to create window surface!"
    );
}
