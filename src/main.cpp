#include "river.h"
#include "window.h"
#include "editor.h"

#if !defined(DEBUG) && defined(_WIN32)
int WinMain()
#else
int main()
#endif
{
    ProjectManifest manifest;
    UserSettings settings;
    EngineData engine;

    initEditor(manifest, settings, engine);
    initGLFW(engine.window, settings.windowWidth, settings.windowHeight, engine.windowName);
    initVulkan(manifest);

    loopEditor();

    cleanupGLFW();
    cleanupEditor();
}
