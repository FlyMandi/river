#pragma once

#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include "river.h"

extern VkExtent2D chooseSwapExtent
(
    const VkSurfaceCapabilitiesKHR  &capabilities,
    GLFWwindow                      *window
);

extern void createSwapchain
(
    EngineData          &engine,
    const UserSettings  &settings
);

extern void recreateSwapchain
(
    EngineData          &engine,
    const UserSettings  &settings
);

extern void createSwapImageViews
(
    EngineData &engine
);

extern void createRenderPass
(
    EngineData &engine
);

extern void cleanupSwapchain
(
    const EngineData &engine
);
