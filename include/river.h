#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include <filesystem>
#include <vector>
#include <fstream>

#define persistent static
#define global_var static
#define internal_f static

static uint32_t currentFrame = 0;

constexpr auto ENGINE_NAME = "River";

global_var VkInstance instance;
global_var std::ofstream logFile;
global_var uint8_t logLevel;

struct ProjectManifest
{
    const char* projectName = "RIV_UNINITIALIZED_CSTRING";
    std::string projectVersion = "RIV_UNINITIALIZED_STRING";
    std::filesystem::path projectRoot = "RIV_UNINITIALIZED_PATH";
    std::filesystem::path projectLog = "RIV_UNINITIALIZED_PATH";

    std::filesystem::path projectModelPath = "RIV_UNINITIALIZED_PATH";
    std::filesystem::path projectTexturePath = "RIV_UNINITIALIZED_PATH";

    std::filesystem::path vertexShader = "RIV_UNINITIALIZED_PATH";
    std::filesystem::path fragmentShader = "RIV_UNINITIALIZED_PATH";
};

struct UserSettings
{
    uint32_t windowHeight;
    uint32_t windowWidth;

    VkPresentModeKHR presentMode;
};

struct EngineData
{
    GLFWwindow *window;
    const char* windowName;

    VkSurfaceKHR surface;
};

enum RiverLogLevel
{
    RIV_LOG_LEVEL_TRACE     = 0,
    RIV_LOG_LEVEL_DEBUG     = 1,
    RIV_LOG_LEVEL_WARN      = 2,
    RIV_LOG_LEVEL_ERROR     = 3,
    RIV_LOG_LEVEL_ASSERT    = 4,
    RIV_LOG_LEVEL_UNDEFINED = 5
};

extern void initVulkan(const ProjectManifest &manifest);
extern void cleanupVulkan();

extern void drawFrame();

extern std::filesystem::path getProjectRoot(const char *rootName);
extern void riverSetupLog(const std::filesystem::path &path);
extern void riverCloseLog();

#ifdef DEBUG
global_var VkDebugUtilsMessengerEXT debugMessenger;

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_KHRONOS_synchronization2",
    // "VK_LAYER_LUNARG_crash_diagnostic",
    "VK_LAYER_LUNARG_monitor",
    "VK_LAYER_RTSS"
};
#endif

const char* riverTranslateVkResult(VkResult code);

extern void riverLog(const std::string_view text, const RiverLogLevel level);
extern void riverAssert(bool condition, const std::string_view assertFailureMsg);
extern void riverAssertVkSuccess(VkResult result, const std::string_view assertFailureMsg);
extern void riverThrow(const std::string_view throwMsg);
