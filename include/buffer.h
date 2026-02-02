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
    VkDeviceSize            bufferSize,
    VkBufferUsageFlags      usageFlags,
    VkMemoryPropertyFlags   memPropertyFlags,
    VkBuffer                &buffer,
    VkDeviceMemory          &bufferMemory,
    std::set<uint32_t>      &uniqueQueueFamilies
);

extern void createDepthResources
(
    const EngineData &engine
);

extern uint32_t findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags);

extern void createVertexBuffer();
extern void createUniformBuffers();

VkFormat findSupportedFormat
(
    const std::vector<VkFormat> &candidates,
    VkImageTiling               tiling,
    VkFormatFeatureFlags        features
);

extern void updateUniformBuffer
(
    const EngineData    &engine,
    uint32_t            currentImage //redundant?
);

extern VkVertexInputBindingDescription getVertexBindingDescription();
extern std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions();
