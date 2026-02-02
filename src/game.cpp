#include "h/game.h"
#include "h/engine.h"
#include "h/window.h"

void initGame(){
    std::ios_base::sync_with_stdio(false);
    createMainMenu();
}

void gameLoop(){
    while(!window_SHOULDCLOSE){
        windowPollEvents();
        drawFrame();
    }
}

void createMainMenu(){

}
