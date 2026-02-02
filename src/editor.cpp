#include "editor.h"
#include "river.h"
#include "window.h"

#include <filesystem>

void initEditor()
{
    //TODO:#40: main menu GUI. buttons:
    //load project
    //create new project
    //exit

    //HACK: currently no project manifest, so all hardcoded.
    riverWindowWidth = 1920;
    riverWindowHeight = 1080;
    riverAppName = "riverTest";
    riverAppVersion = "0.0.0";
    riverAppRoot = getProjectRoot("river");
}

void loopEditor()
{
    //TODO:#39: draw & exist only in specified viewport area
    //be able to have multiple viewports that can be paused (frozen) and resumed at will
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        drawFrame();
    }
}

void cleanupEditor()
{
    //NOTE: currently nothing else to do here.
    //this will change when river gets proj files.
    cleanupVulkan();
}
