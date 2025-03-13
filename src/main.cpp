#include "h/game.h"
#include "h/engine.h"
#include "GLFW/glfw3.h"

void gameLoop(){
    while(!appShouldClose()){
       glfwPollEvents(); 
    }
}

int main(){
    initGLFW();
    initVulkan();

    gameLoop();

    cleanupVulkan();
    cleanupGLFW();
}
