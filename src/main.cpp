#include "river.h"
#include "window.h"
#include "editor.h"

#if !defined(DEBUG) && defined(_WIN32)

int WinMain(){
    initEditor();
    initGLFW();
    initVulkan();

    loopEditor();

    cleanupEditor();
    cleanupGLFW();
    cleanupVulkan();

    return 0;
}

#else

int main(){
    initEditor();
    initGLFW();
    initVulkan();

    loopEditor();

    cleanupEditor();
    cleanupGLFW();
    cleanupVulkan();

    return 0;
}

#endif
