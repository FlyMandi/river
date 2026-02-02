#pragma once

#include "Engine_Window.h"

namespace engine{

class Cnake {
 public:
    static constexpr int w_WIDTH = 1280;
    static constexpr int w_HEIGHT = 720;

    void run();

 private:
    EngineWindow engineWindow{w_WIDTH, w_HEIGHT, "I did it!"};
};

}
