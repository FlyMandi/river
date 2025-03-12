#include "h/cnake.h"
#include "h/engine.h"
#include "GLFW/glfw3.h"

#include <cstdlib>
#include <cstring>

const char *version = "Cnake 0.0.0";

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

GLFWwindow *window;

void run(){
    initWindow();
    initVulkan();
    gameLoop();
    cleanup();
}

void gameLoop(){
    while(!glfwWindowShouldClose(window)){
       glfwPollEvents(); 
    }
}

void initWindow(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, version, nullptr, nullptr);
}

int main(){
    try{
        run();
    }catch(const std::exception &e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
