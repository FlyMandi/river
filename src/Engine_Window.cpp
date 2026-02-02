#include "Engine_Window.h"
#include "GLFW/glfw3.h"

namespace engine {
    
EngineWindow::EngineWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
    initWindow(); 
}

EngineWindow::~EngineWindow(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

void EngineWindow::initWindow() { 
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

} // namespace engine
