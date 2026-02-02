#include "vulkan/vulkan_core.h"

#include "river.h"
#include "device.h"
#include <cstring>
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

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

uint32_t findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags)
{
    for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i){
        if(typeFilter & (1 << i) && ((deviceMemoryProperties.memoryTypes[i].propertyFlags & flags) == flags)){
            return i;
        }
    }

    #ifdef DEBUG
        printDebugLog("failed to find suitable memory type!");
    #endif
    throw std::runtime_error("failed to find suitable memory type!");
}

void createVertexBuffer()
{
    VkBufferCreateInfo vertexBufferInfo{};
    vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.size = sizeof(vertices[0]) * vertices.size(); 
    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(logicalDevice, &vertexBufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS){
        #ifdef DEBUG
            printDebugLog("failed to create vertex buffer!");
        #endif
        throw std::runtime_error("failed to create vertex buffer!");
    }
    #ifdef DEBUG
        printDebugLog('\0', "created vertex buffer.", '\n');
    #endif

    VkMemoryRequirements vertexBufferMemoryRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, vertexBuffer, &vertexBufferMemoryRequirements);

    VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = vertexBufferMemoryRequirements.size;
    allocInfo.memoryTypeIndex = findSuitableMemoryType(vertexBufferMemoryRequirements.memoryTypeBits, flags);

    if(vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS){
        #ifdef DEBUG
            printDebugLog("failed to allocate vertex buffer memory!");
        #endif
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }
    #ifdef DEBUG
        printDebugLog('\0', "allocated vertex buffer memory: ");
        printDebugLog(allocInfo.allocationSize);
        printDebugLog("B", '\n');
    #endif

    vkBindBufferMemory(logicalDevice, vertexBuffer, vertexBufferMemory, 0);

    void* vertexBufferBegin;
    vkMapMemory(logicalDevice, vertexBufferMemory, 0, vertexBufferInfo.size, 0, &vertexBufferBegin);
    std::memcpy(vertexBufferBegin, vertices.data(), (size_t)vertexBufferInfo.size);
    vkUnmapMemory(logicalDevice, vertexBufferMemory);
}
