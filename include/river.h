#pragma once

#if defined(DEBUG) || defined(_DEBUG)
    const bool build_DEBUG = true;
#else
    const bool build_DEBUG = false;
#endif

namespace river{

inline const char *engineName = "River";
inline const char *appName;
inline const char *appVersion;

void initVulkan();
void cleanupVulkan();

void drawFrame();

void getProjectRoot(const char* rootName);

}
