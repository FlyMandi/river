#pragma once

#include "Engine_Window.h"
#include <string>

namespace engine{

class Cnake {
 public:
    static constexpr int w_WIDTH = 1920;
    static constexpr int w_HEIGHT = 1080;

    void run();

 private:
    const std::string m_Version = "0.0.1";

 private:
    EngineWindow engineWindow{w_WIDTH, w_HEIGHT, "Cnake " + m_Version}; 
};

}
