#include "river.h"
#include "window.h"
#include "editor.h"

#if !defined(DEBUG) && defined(_WIN32)

int WinMain()
{
    initEditor();
    initGLFW();
    initVulkan();

    loopEditor();

    cleanupGLFW();
    cleanupEditor();
}

#else

int main()
{
    projectManifest manifest;
    userSettings settings;

    initEditor(manifest);
    initGLFW();
    initVulkan(manifest);

    loopEditor();

    cleanupGLFW();
    cleanupEditor();
}

#endif
