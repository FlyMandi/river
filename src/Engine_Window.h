#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace engine {

class EngineWindow {
 public:
    EngineWindow(int w, int h, std::string name);
    ~EngineWindow();

    bool shouldClose() { return glfwWindowShouldClose(window); }

 private:
    void initWindow();
    const int width;
    const int height;

    std::string windowName;
    GLFWwindow *window;

};

}  // namespace engine
