#include "h/game.h"
#include "h/engine.h"
#include "GLFW/glfw3.h"

#include <cstdlib>
#include <cstring>

void run(){
    initGLFW();
    initVulkan();

    gameLoop();

    cleanupVulkan();
    cleanupGLFW();
}

void gameLoop(){
    while(!appShouldClose()){
       glfwPollEvents(); 
    }
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
