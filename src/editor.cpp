#include "editor.h"
#include "river.h"
#include "window.h"

void initEditor()
{
    //TODO:#40: main menu GUI. buttons:
    //load project
    //create new project
    //exit

    //HACK: currently no project manifest, so all hardcoded.
    projectName = "riverTest";
    projectVersion = "0.0.0";

    riverWindowWidth = 1920;
    riverWindowHeight = 1080;

    riverModelPath = "assets/models/WB_Vase.obj";
    riverTexturePath = "assets/textures/WB_Vase_Mat_Base_color.jpg";

    getProjectRoot("river");
    riverSetupLog();
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
    cleanupVulkan();
    riverCloseLog();
}
