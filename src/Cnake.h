#pragma once

#include "Engine_Window.h"
#include "Engine_Pipeline.h"
#include "Engine_Device.h"
#include "Engine_Swapchain.h"
#include "vulkan/vulkan_core.h"
#include <string>

#include <vector>
#include <memory>

namespace engine{

class Cnake {
    const std::string m_Version = "0.0.1";

    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    EngineWindow engineWindow{w_WIDTH, w_HEIGHT, "Cnake " + m_Version};
    EngineDevice engineDevice{engineWindow};
    EngineSwapChain engineSwapChain{engineDevice, engineWindow.getExtent()};
    std::unique_ptr<EnginePipeline> enginePipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;

 public:
    static constexpr int w_WIDTH = 1920;
    static constexpr int w_HEIGHT = 1080;

    Cnake();
    ~Cnake();
    Cnake(const Cnake &) = delete; 
    Cnake &operator=(const Cnake &) = delete;

    void run();
};

}
