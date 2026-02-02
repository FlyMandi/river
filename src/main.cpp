#include "river.h"
#include "window.h"
#include "editor.h"

#ifndef _WIN32

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

#else

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

#endif
