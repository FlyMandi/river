#pragma once

#include "river.h"

extern void initEditor
(
    EngineData          &engine,
    ProjectManifest     &manifest,
    const UserSettings  &settings
);

extern void loopEditor
(
    EngineData          &engine,
    const UserSettings  &settings
);

extern void cleanupEditor(EngineData &engine);
