#include "editor.h"
#include "river.h"
#include <filesystem>

void initEditor
(
    EngineData          &engine,
    ProjectManifest     &manifest,
    const UserSettings  &settings
){
    //TODO:#40: main menu GUI. buttons:
    //load project
    //create new project
    //exit

    //HACK: currently no project manifest, so all hardcoded.
    //later acquire from files

    manifest.projectName = "riverTest";
    manifest.projectVersion = "0.0.0";

    logLevel = 0;

    engine.windowName = manifest.projectName + " " + manifest.projectVersion;

    manifest.projectRoot = getProjectRoot("river");
    manifest.projectLog = manifest.projectRoot / "log" / "river.log";

    manifest.vertexShader = manifest.projectRoot / "bin" / "vertTest.vert.spv";
    manifest.fragmentShader = manifest.projectRoot / "bin" / "fragTest.frag.spv";

    std::filesystem::path models = manifest.projectRoot / "assets" / "models";
    std::filesystem::path textures = manifest.projectRoot / "assets" / "textures";

    manifest.projectModelPath = std::filesystem::canonical(models / "WB_Vase.obj");
    manifest.projectTexturePath = std::filesystem::canonical(textures / "WB_Vase_Mat_Base_color.jpg");

    riverSetupLog(manifest.projectLog);
}

void loopEditor
(
    EngineData          &engine,
    const UserSettings  &settings
){
    //TODO:#39: draw & exist only in specified viewport area
    //be able to have multiple viewports that can be paused (frozen) and resumed at will
    while(!glfwWindowShouldClose(engine.window))
    {
        glfwPollEvents();
        drawFrame(engine.surface, engine.window, settings.presentMode);
    }
}

void cleanupEditor(EngineData &engine)
{
    cleanupVulkan(engine);
    riverCloseLog();
}
