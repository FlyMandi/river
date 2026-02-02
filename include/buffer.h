#pragma once

#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "river.h"

#include <set>
#include <vector>
#include <array>
#include <filesystem>

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

//TODO:#57: rid of inline global scope vars
inline std::vector<Vertex> vertices;
inline std::vector<uint32_t> vertexIndices;

inline VkBuffer vertexBuffer;
inline VkDeviceMemory vertexBufferMemory;
inline VkDeviceSize vertSize;

inline VkImage depthImage;
inline VkDeviceMemory depthImageMemory;
inline VkImageView depthImageView;

inline std::vector<VkBuffer> uniformBuffers{};
inline std::vector<VkDeviceMemory> uniformBuffersMemory{};
inline std::vector<void*> uniformBuffersMapped{};

extern void loadModel(const std::filesystem::path &modelPath);

extern void createBuffer
(
    const EngineData            &engine,
    const VkDeviceSize          &bufferSize,
    const VkBufferUsageFlags    &usageFlags,
    const VkMemoryPropertyFlags &memPropFlags,
    VkBuffer                    &buffer,
    VkDeviceMemory              &bufferMemory,
    const std::set<uint32_t>    &uniqueQueueFamilies
);

extern void createDepthResources
(
    const EngineData &engine
);

extern uint32_t findSuitableMemoryType
(
    const EngineData            &engine,
    const uint32_t              &typeFilter,
    const VkMemoryPropertyFlags &flags
);

extern void createVertexBuffer
(
    const EngineData &engine
);

extern void createUniformBuffers
(
    const EngineData &engine
);

extern VkFormat findSupportedFormat
(
    const EngineData            &engine,
    const std::vector<VkFormat> &candidates,
    const VkImageTiling         &tiling,
    const VkFormatFeatureFlags  &features
);

extern void updateUniformBuffer
(
    const EngineData    &engine,
    uint32_t            currentImage //redundant?
);

extern VkVertexInputBindingDescription getVertexBindingDescription();
extern std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions();
