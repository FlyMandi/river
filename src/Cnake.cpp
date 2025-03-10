#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class CnakeApp{
    void initWindow(){

    }

    void initVulkan(){

    }

    void gameLoop(){

    }

    void cleanup(){

    }

 public:
    void run(){
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
