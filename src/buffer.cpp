#include <unordered_map>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtx/hash.hpp" // IWYU pragma: keep (for some reason clang doesn't recognize I need it below)
#include "vulkan/vulkan_core.h"

#include "pipeline.h"
#include "river.h"
#include "device.h"
#include "swapchain.h"
#include "buffer.h"
#include "image.h"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <set>

template<> struct std::hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const
    {
        return  ((hash<glm::vec3>()(vertex.position) ^
                    (hash<glm::vec3>()(vertex.colour) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.textureCoordinate) << 1);
    }
};

void loadModel(const std::filesystem::path &modelPath)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string error;

    riverAssert
    (
        tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &error, modelPath.string().c_str()),
        (warn + error)
    );

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for(const tinyobj::shape_t &shape : shapes)
    {
        for(const tinyobj::index_t &index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.position =
            {
                attributes.vertices[3 * index.vertex_index + 0],
                attributes.vertices[3 * index.vertex_index + 1],
                attributes.vertices[3 * index.vertex_index + 2] - 0.25
            };

            vertex.textureCoordinate =
            {
                attributes.texcoords[2 * index.texcoord_index + 0],
                1.0f - attributes.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.colour = { 1.0f, 1.0f, 1.0f };

            if(!uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.emplace_back(vertex);
            }

            vertexIndices.emplace_back(uniqueVertices[vertex]);
        }
    }
}

VkVertexInputBindingDescription getVertexBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, colour);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, textureCoordinate);

    return attributeDescriptions;
}

uint32_t findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags)
{
    for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
    {
        if( typeFilter & (1 << i) &&
            ((deviceMemoryProperties.memoryTypes[i].propertyFlags & flags) == flags))
        {
            return i;
        }
    }

    riverLog("failed to find suitable memory type!", RIV_LOG_LEVEL_ERROR);
    return UINT32_MAX;
}

void createBuffer
(
    VkDeviceSize            bufferSize,
    VkBufferUsageFlags      usageFlags,
    VkMemoryPropertyFlags   memPropertyFlags,
    VkBuffer                &buffer,
    VkDeviceMemory          &bufferMemory,
    std::set<uint32_t>      &uniqueQueueFamilies
){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlags;

    if(uniqueQueueFamilies.size() > 1)
    {
        bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    }else{
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    std::vector<uint32_t> queueFamilyIndices(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());

    bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    bufferInfo.pQueueFamilyIndices = queueFamilyIndices.data();

    riverAssertVkSuccess
    (
        vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer),
        "failed to create buffer!"
    );

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findSuitableMemoryType(memRequirements.memoryTypeBits, memPropertyFlags);

    riverAssertVkSuccess
    (
        vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory),
        "failed to allocate buffer!"
    );

    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
{
    VkCommandBuffer commandBuffer = setupCommandBuffer(transferCommandPool);

    VkBufferCopy transferCopyRegion{};
    transferCopyRegion.size = bufferSize;
    transferCopyRegion.srcOffset = 0;
    transferCopyRegion.dstOffset = 0;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &transferCopyRegion);

    flushCommandBuffer(commandBuffer, transferCommandPool, transferQueue);
}

void createVertexBuffer()
{
    vertSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize indexSize = sizeof(vertexIndices[0]) * vertexIndices.size();

    VkDeviceSize bufferSize = vertSize + indexSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    std::set<uint32_t> queueFamilies =
    {
        logicalQueueFamilies.graphicsIndex,
        logicalQueueFamilies.transferIndex
    };

    createBuffer
    (
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory,
        queueFamilies
    );

    void* pData;

    vkMapMemory(logicalDevice, stagingBufferMemory, 0, vertSize, 0, &pData);
    ::memcpy(pData, vertices.data(), static_cast<size_t>(vertSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    vkMapMemory(logicalDevice, stagingBufferMemory, vertSize, indexSize, 0, &pData);
    ::memcpy(pData, vertexIndices.data(), static_cast<size_t>(indexSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    createBuffer
    (
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,
        vertexBufferMemory,
        queueFamilies
    );

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void createUniformBuffers()
{
    VkDeviceSize uniformBufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        std::set<uint32_t> uniqueFamilyIndices =
        {
            logicalQueueFamilies.graphicsIndex,
            logicalQueueFamilies.presentIndex
        };

        createBuffer
        (
            uniformBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniformBuffers[i],
            uniformBuffersMemory[i],
            uniqueFamilyIndices
        );

        vkMapMemory(logicalDevice, uniformBuffersMemory[i], 0, uniformBufferSize, 0, &uniformBuffersMapped[i]);
    };
}

void updateUniformBuffer(uint32_t currentImage)
{
    persistent std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();

    std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject uniformBuffer{};
    uniformBuffer.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    uniformBuffer.view = glm::lookAt(glm::vec3(1.0f, 0.0f, 0.3f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    uniformBuffer.projection =  glm::perspective
                                (
                                    glm::radians(35.0f),
                                    swapchainExtent.width / static_cast<float>(swapchainExtent.height),
                                    0.1f,
                                    256.0f
                                );

    uniformBuffer.projection[1][1] *= -1;

    ::memcpy(uniformBuffersMapped[currentImage], &uniformBuffer, sizeof(uniformBuffer));
}

VkFormat findSupportedFormat
(
    const std::vector<VkFormat> &candidates,
    VkImageTiling               tiling,
    VkFormatFeatureFlags        features
){
    for(VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if( tiling == VK_IMAGE_TILING_LINEAR &&
            ((props.linearTilingFeatures & features) == features))
        {
            return format;
        }
        else if(tiling == VK_IMAGE_TILING_OPTIMAL &&
                ((props.optimalTilingFeatures & features) == features))
        {
            return format;
        }
    }
    riverThrow("failed to find supported format!");
    return VK_FORMAT_MAX_ENUM; //to silence clang
}

void createDepthResources()
{
    const std::vector<VkFormat> candidates =
    {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    VkFormat depthFormat =  findSupportedFormat
                            (
                                candidates,
                                VK_IMAGE_TILING_OPTIMAL,
                                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                            );
    createImage
    (
        swapchainExtent.width,
        swapchainExtent.height,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory
    );

    depthImageView =    createImageView
                        (
                            depthImage,
                            depthFormat,
                            VK_IMAGE_ASPECT_DEPTH_BIT
                        );

    transitionImageLayout
    (
        depthImage,
        depthFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );
}
