#include "editor.h"
#include "river.h"
#include "window.h"

void initEditor(){
    //TODO: main menu GUI. buttons:
    //load project
    //create new project
    //exit
    
    //TODO:
    //upon loading or creating, load layout
    //this is all layout info that should be stored in a manifest somewhere
    WIDTH = 1920;
    HEIGHT = 1080;
    appName = "riverTest";
    appVersion = "0.0.0";

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
