#pragma once

#include "river.h"

extern SwapchainSupportDetails querySwapchainSupport
(
    const VkPhysicalDevice  &physicalDevice,
    const VkSurfaceKHR      &surface
);

extern QueueFamilyIndices findQueueFamilies
(
    const EngineData &engine
);

extern void pickPhysicalDevice
(
    EngineData &engine
);

extern void createLogicalDevice
(
    EngineData &engine
);

extern VkSampleCountFlagBits getMaxMSAASamples
(
    const EngineData &engine
);
