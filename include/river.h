#pragma once

namespace river{

inline const char *engineName = "River";
inline const char *appName;
inline const char *appVersion;

void initVulkan();
void cleanupVulkan();

void drawFrame();

void getProjectRoot(const char* rootName);

}
