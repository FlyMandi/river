#include "editor.h"
#include "river.h"
#include "window.h"

void initEditor(){
    //TODO: main menu GUI. buttons:
    //load project
    //create new project
    //exit
    
    //upon loading or creating, load layout
    initVulkan();
}

void loopEditor(){
    //TODO: draw & exist only in specified viewport area
    //be able to have multiple viewports that can be paused (frozen) and resumed at will
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        drawFrame();
    }
}

void cleanupEditor(){
    //TODO: shutdown safely, ask for saving
    //shutdown vulkan
}
