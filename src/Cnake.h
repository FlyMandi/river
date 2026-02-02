#pragma once

#include "Engine_Window.h"
#include "Engine_Pipeline.h"
#include "Engine_Device.h"
#include <string>

namespace engine{

class Cnake {
 public:
    static constexpr int w_WIDTH = 1920;
    static constexpr int w_HEIGHT = 1080;

    void run();

 private:
    const std::string m_Version = "0.0.2";

 private:
    EngineWindow engineWindow{w_WIDTH, w_HEIGHT, "Cnake " + m_Version};
    EngineDevice engineDevice{engineWindow};
    EnginePipeline enginePipeline{engineDevice, EnginePipeline::defaultPipelineConfigInfo(w_WIDTH, w_HEIGHT),"T:/Repository/Cnake/src/shaders/simple_shader.vert.spv", "T:/Repository/Cnake/src/shaders/simple_shader.frag.spv"};
};

}
