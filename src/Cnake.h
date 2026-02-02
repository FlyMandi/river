#pragma once

#include "Engine_Window.h"
#include "Engine_Pipeline.h"
#include "Engine_Device.h"
#include <string>

namespace engine{

class Cnake {
    const std::string m_Version = "0.0.1";

    EngineWindow engineWindow{w_WIDTH, w_HEIGHT, "Cnake " + m_Version};
    EngineDevice engineDevice{engineWindow};
    EnginePipeline enginePipeline{engineDevice, EnginePipeline::defaultPipelineConfigInfo(w_WIDTH, w_HEIGHT), "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv"};

 public:
    static constexpr int w_WIDTH = 1920;
    static constexpr int w_HEIGHT = 1080;

    void run();
};

}
