#include "river.h"
#include "window.h"
#include "editor.h"

#if !defined(DEBUG) && defined(_WIN32)
int WinMain()
#else
int main()
#endif
{
    EngineData      engine;
    ProjectManifest manifest;
    UserSettings    settings;

    riverLog
    (
        std::format("engine struct size: {}", std::to_string(sizeof(EngineData))),
        RIV_LOG_LEVEL_DEBUG
    );

    //HACK: evtl be loaded from file inside initEditor
    settings.windowWidth = 1920;
    settings.windowHeight = 1080;

    initEditor(engine, manifest, settings);
    initGLFW(engine, settings);
    initVulkan(engine, manifest, settings);

    loopEditor(engine, settings);

    cleanupGLFW(engine.window);
    cleanupEditor(engine);
}
