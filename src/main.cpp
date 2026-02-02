#include "h/game.h"
#include "h/engine.h"
#include "GLFW/glfw3.h"

void gameLoop(){
    while(!appShouldClose()){
       glfwPollEvents(); 
    }
}

int main(){
    std::ios_base::sync_with_stdio(false);

    initGLFW();
    initVulkan();

    gameLoop();

    cleanupVulkan();
    cleanupGLFW();
}
