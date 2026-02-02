#pragma once

#include "GLFW/glfw3.h"
#include "river.h"

extern void initGLFW
(
    EngineData              &engine,
    const UserSettings      &settings
);

extern void cleanupGLFW(GLFWwindow *window);

extern void createSurface(EngineData &engine);
