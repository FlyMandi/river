#include "Cnake.h"
#include "GLFW/glfw3.h"

namespace engine{

void Cnake::run(){

    while(!engineWindow.shouldClose()){
        glfwPollEvents();
    }

}

}
