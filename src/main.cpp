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

    cleanupEditor();
    cleanupGLFW();
    cleanupVulkan();
}

#else

int main()
{
    initEditor();
    initGLFW();
    initVulkan();

    loopEditor();

    cleanupEditor();
    cleanupGLFW();
    cleanupVulkan();
}

#endif
