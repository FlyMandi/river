#include "editor.h"
#include "river.h"
#include <filesystem>

void initEditor(ProjectManifest &manifest, UserSettings &settings, EngineData &engine)
{
    //TODO:#40: main menu GUI. buttons:
    //load project
    //create new project
    //exit

    //HACK: currently no project manifest, so all hardcoded.
    //later acquire from files, for now hardcoded

    manifest.projectName = "riverTest";
    manifest.projectVersion = "0.0.0";

    logLevel = 0;

    std::string name = manifest.projectName;
    name += " " + manifest.projectVersion;
    engine.windowName = name.c_str();

    manifest.projectRoot = getProjectRoot("river");
    manifest.projectLog = manifest.projectRoot / "log" / "river.log";

    manifest.projectModelPath = std::filesystem::canonical(manifest.projectRoot / "assets/models/WB_Vase.obj");
    manifest.projectTexturePath = std::filesystem::canonical(manifest.projectRoot / "assets/textures/WB_Vase_Mat_Base_color.jpg");

    riverSetupLog(manifest.projectLog);
}

void loopEditor(EngineData &engine)
{
    //TODO:#39: draw & exist only in specified viewport area
    //be able to have multiple viewports that can be paused (frozen) and resumed at will
    while(!glfwWindowShouldClose(engine.window))
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
