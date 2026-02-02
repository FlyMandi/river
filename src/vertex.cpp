#include "vulkan/vulkan_core.h"

#include "river.h"
#include "device.h"
#include "vertex.h"

const std::vector<Vertex> vertices =
{
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

VkVertexInputBindingDescription getVertexBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{}; 
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> getVertexAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 1;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

uint32_t findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags)
{
    for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i){
        if(typeFilter & (1 << i) && ((deviceMemoryProperties.memoryTypes[i].propertyFlags & flags) == flags)){
            return i;
        }
    }

    printDebugLog("failed to find suitable memory type!");
    throw std::runtime_error("failed to find suitable memory type!");
}

void createVertexBuffer()
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size(); 

    bufferInfo.usage = VK_BUFFER_USAGE_2_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS){
        printDebugLog('\0', "failed to create vertex buffer!");
        throw std::runtime_error("failed to create vertex buffer!");
    }
}
