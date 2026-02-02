#pragma once

#include "river.h"

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

extern void createRenderPass
(
    EngineData &engine
);

extern void cleanupSwapchain
(
    const EngineData &engine
);
