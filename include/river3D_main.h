#pragma once

#include "imgsurf_main.h"

#include <vulkan/vulkan_core.h>

#define persistent  static
#define global      static
#define internal    static

#define ENGINE_NAME = "river3D";
//maybe get rid of this in the future
#define MAX_FRAMES_IN_FLIGHT = 2;

#ifdef DEBUG
    #define RIV_ASSERT              riverAssert
    #define RIV_ASSERT_VK_SUCCESS   riverAssertVkSuccess
#else
    #define RIV_ASSERT              0 && riverAssert
    #define RIV_ASSERT_VK_SUCCESS   0 && riverAssertVkSuccess
#endif

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    uint32_t graphicsIndex = UINT32_MAX;
    uint32_t transferIndex = UINT32_MAX;
    uint32_t presentIndex  = UINT32_MAX;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 colour;
    glm::vec2 textureCoordinate;

    bool operator==(const Vertex& other) const
    {
        return  position            == other.position   &&
                colour              == other.colour     &&
                textureCoordinate   == other.textureCoordinate;
    }
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct ProjectManifest
{
    ProjectManifest& operator=(const ProjectManifest&) = delete;
    ProjectManifest& operator=(const ProjectManifest&&) = delete;

    std::string             projectName         = "RIV_UNINITIALIZED_STRING";
    std::string             projectVersion      = "RIV_UNINITIALIZED_STRING";
    std::filesystem::path   projectRoot         = "RIV_UNINITIALIZED_PATH";
    std::filesystem::path   projectLog          = "RIV_UNINITIALIZED_PATH";

    std::filesystem::path   projectModelPath    = "RIV_UNINITIALIZED_PATH";
    std::filesystem::path   projectTexturePath  = "RIV_UNINITIALIZED_PATH";

    std::filesystem::path   vertexShader        = "RIV_UNINITIALIZED_PATH";
    std::filesystem::path   fragmentShader      = "RIV_UNINITIALIZED_PATH";
};

struct UserSettings
{
    UserSettings& operator=(const UserSettings&) = delete;
    UserSettings& operator=(const UserSettings&&) = delete;

    uint32_t            windowHeight;
    uint32_t            windowWidth;

    VkPresentModeKHR    presentMode;
};

//maybe chop this up in swapchain / device / pipeline / buffer structs if it gets too big

struct EngineData
{
    EngineData& operator=(const EngineData&) = delete;
    EngineData& operator=(const EngineData&&) = delete;

    #ifdef DEBUG
        VkDebugUtilsMessengerEXT debugMessenger;
    #endif

    GLFWwindow                          *window;
    std::string                         windowName;

    VkInstance                          instance;
    VkSurfaceKHR                        surface;

    QueueFamilyIndices                  logicalQueueFamilies;
    VkPhysicalDevice                    physicalDevice;
    VkPhysicalDeviceProperties          deviceProperties;
    VkPhysicalDeviceMemoryProperties    deviceMemoryProperties;
    VkPhysicalDeviceFeatures            deviceFeatures;

    VkDevice                            logicalDevice;

    VkSwapchainKHR                      swapchain;
    VkFormat                            swapchainImageFormat;
    VkExtent2D                          swapchainExtent;

    uint32_t                            swapchainImageCount;
    std::vector<VkImage>                swapchainImages{};
    std::vector<VkImageView>            swapchainImageViews{};
    std::vector<VkFramebuffer>          swapchainFramebuffers{};

    VkRenderPass                        renderPass;
    VkPipeline                          graphicsPipeline;
    VkPipelineLayout                    graphicsPipelineLayout;

    VkDescriptorSetLayout               descriptorSetLayout;
    std::vector<VkDescriptorSet>        descriptorSets{};

    VkCommandPool                       graphicsCommandPool;
    VkCommandPool                       transferCommandPool;
    VkDescriptorPool                    descriptorPool;

    std::vector<VkCommandBuffer>        commandBuffers{};

    VkQueue                             graphicsQueue;
    VkQueue                             presentQueue;
    VkQueue                             transferQueue;

    std::vector<VkSemaphore>            imageReadyForWriteSemaphores{VK_NULL_HANDLE};
    std::vector<VkSemaphore>            imageReadyForPresentSemaphores{VK_NULL_HANDLE};
    VkSemaphore                         acquireSemaphore = VK_NULL_HANDLE;

    std::vector<VkFence>                inFlightFences{VK_NULL_HANDLE};

    VkBool32                            framebufferResized = VK_FALSE;

    std::vector<Vertex>                 vertices;
    std::vector<uint32_t>               vertexIndices;

    VkBuffer                            vertexBuffer;
    VkDeviceMemory                      vertexBufferMemory;
    VkDeviceSize                        vertSize;

    VkImage                             depthImage;
    VkDeviceMemory                      depthImageMemory;
    VkImageView                         depthImageView;

    std::vector<VkBuffer>               uniformBuffers{};
    std::vector<VkDeviceMemory>         uniformBuffersMemory{};
    std::vector<void*>                  uniformBuffersMapped{};

    uint32_t                            mipLevels;
    VkImage                             textureImage;
    VkImageView                         textureImageView;
    VkDeviceMemory                      textureImageMemory;

    VkSampler                           textureSampler;

    VkSampleCountFlagBits               msaaSamples;
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

extern void initVulkan
(
    EngineData              &engine,
    const ProjectManifest   &manifest,
    const UserSettings      &settings
);

extern void cleanupVulkan
(
    EngineData &engine
);

extern void drawFrame
(
    EngineData          &engine,
    const UserSettings  &settings
);

extern std::filesystem::path getProjectRoot
(
    const char *rootName
);

extern void riverSetupLog
(
    const std::filesystem::path &path
);

extern void riverCloseLog();

#ifdef DEBUG

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_KHRONOS_synchronization2",
    // "VK_LAYER_LUNARG_crash_diagnostic",
    "VK_LAYER_LUNARG_monitor",
    "VK_LAYER_RTSS"
};
#endif

extern void riverLog
(
    const std::string_view  text,
    const RiverLogLevel     &level
);

extern bool riverAssert
(
    const bool              &condition,
    const std::string_view  assertFailureMsg
);

extern bool riverAssertVkSuccess
(
    const VkResult          &result,
    const std::string_view  assertFailureMsg
);

extern void riverThrow
(
    const std::string_view throwMsg
);
