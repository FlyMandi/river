#pragma once

#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include "river.h"

//maybe get rid of this in the future
constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

inline uint32_t swapchainImageCount;

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
    const EngineData &engine
);

extern void cleanupSwapchain
(
    const EngineData &engine
);
