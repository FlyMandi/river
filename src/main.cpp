#include "window.h"
#include "editor.h"

int main(){
    initGLFW();
    initEditor();

    loopEditor();

    cleanupEditor();
    cleanupGLFW();
}
