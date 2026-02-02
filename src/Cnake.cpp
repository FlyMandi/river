#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class CnakeApp{
    const char *version = "Cnake 0.0.0";
    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

    GLFWwindow *window;

    void initWindow(){
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, version, nullptr, nullptr);
    }

    void initVulkan(){

    }

    void gameLoop(){
        while(!glfwWindowShouldClose(window)){
           glfwPollEvents(); 
        }
    }

    void cleanup(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

 public:
    void run(){
        initWindow();
        initVulkan();
        gameLoop();
        cleanup();
    }
};

int main(){
    CnakeApp app;

    try{
        app.run();
    }catch(const std::exception &e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
