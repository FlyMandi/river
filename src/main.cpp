#include "h/window.h"
#include "h/game.h"
#include "h/engine.h"

int main(){

    initGLFW();
    initVulkan();

    initGame();
    gameLoop();

    cleanupVulkan();
    cleanupGLFW();
}
