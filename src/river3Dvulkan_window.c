#include "river3D_main.h"

internal void framebufferResizeCallback
(
    GLFWwindow  *window,
    int         width,
    int         height
){
    EngineData *engine = static_cast<EngineData*>(glfwGetWindowUserPointer(window));

    engine->framebufferResized = VK_TRUE;
}

void createSurface(EngineData &engine)
{
    //TODO:#39: find out if I can create a surface smaller than the window.
    //GLFW sub-windows? or Vulkan scissor?

    VkResult result = glfwCreateWindowSurface(engine.instance, engine.window, 0, &engine.surface);
    RIV_ASSERT_VK_SUCCESS(result,  "failed to create window surface!");
}
