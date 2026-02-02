#pragma once

#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>

#include <set>
#include <vector>
#include <array>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 colour;
    glm::vec2 texCoord;
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

inline std::vector<Vertex> vertices{};
inline std::vector<uint32_t> indices{};

inline VkBuffer vertexBuffer;
inline VkDeviceMemory vertexBufferMemory;
inline VkDeviceSize vertSize;

inline VkImage depthImage;
inline VkDeviceMemory depthImageMemory;
inline VkImageView depthImageView;

inline std::vector<VkBuffer> uniformBuffers{};
inline std::vector<VkDeviceMemory> uniformBuffersMemory{};
inline std::vector<void*> uniformBuffersMapped{};

extern void loadModel();

extern void createBuffer
(
    VkDeviceSize            bufferSize,
    VkBufferUsageFlags      usageFlags,
    VkMemoryPropertyFlags   memPropertyFlags,
    VkBuffer                &buffer,
    VkDeviceMemory          &bufferMemory,
    std::set<uint32_t>      &uniqueQueueFamilies
);

extern void createDepthResources();

extern uint32_t findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags);

extern void createVertexBuffer();
extern void createUniformBuffers();

VkFormat findSupportedFormat
(
    const std::vector<VkFormat> &candidates,
    VkImageTiling               tiling,
    VkFormatFeatureFlags        features
);

extern void updateUniformBuffer(uint32_t currentImage);

extern VkVertexInputBindingDescription getVertexBindingDescription();
extern std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions();
