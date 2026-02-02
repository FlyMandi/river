#include "h/cnake.h"
#include "h/engine.h"
#include "GLFW/glfw3.h"

#include <cstdlib>
#include <cstring>

void CnakeApp::gameLoop(){
    while(!glfwWindowShouldClose(window)){
       glfwPollEvents(); 
    }
}

void CnakeApp::run(){
    initWindow();

    Engine engine(window);
    engine.initVulkan();

    gameLoop();
    engine.cleanup();
}

int main(){
    CnakeApp app;

    try{
        app.run();
    }catch(const std::exception &e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
