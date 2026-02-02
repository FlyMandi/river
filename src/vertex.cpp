#include "vulkan/vulkan_core.h"

#include "river.h"
#include "device.h"
#include <cstring>
#include <set>
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

void createBuffer(  VkDeviceSize            bufferSize, 
                    VkBufferUsageFlags      usageFlags, 
                    VkMemoryPropertyFlags   memPropertyFlags, 
                    VkBuffer                &buffer, 
                    VkDeviceMemory          &bufferMemory,
                    std::set<uint32_t>      &uniqueQueueFamilies)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlags;

    if(uniqueQueueFamilies.size() == 1){
        bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    }else{
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    static std::vector<uint32_t> queueFamilyIndices(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());

    bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    bufferInfo.pQueueFamilyIndices = queueFamilyIndices.data();

    if(vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS){
        #ifdef DEBUG
            printDebugLog('\0', "failed to create buffer! usage flags: ");
            printDebugLog(usageFlags);
        #endif
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findSuitableMemoryType(memRequirements.memoryTypeBits, memPropertyFlags);

    if(vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS){
        #ifdef DEBUG
            printDebugLog("failed to allocate buffer memory! usage flags: ");
            printDebugLog(usageFlags);
        #endif
        throw std::runtime_error("failed to allocate buffer memory!");
    }
    #ifdef DEBUG
        printDebugLog('\0', "allocated buffer memory: ");
        printDebugLog(allocInfo.allocationSize);
        printDebugLog("B, usage flags: ");
        printDebugLog(usageFlags, '\n');
    #endif

    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

void createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    static std::set<uint32_t> queueFamilies = 
    {
        logicalQueueFamilies.graphicsIndex,
        logicalQueueFamilies.transferIndex
    };

    createBuffer(   bufferSize, 
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    vertexBuffer,
                    vertexBufferMemory,
                    queueFamilies);

    void* vertexBufferBegin;
    vkMapMemory(logicalDevice, vertexBufferMemory, 0, bufferSize, 0, &vertexBufferBegin);
    std::memcpy(vertexBufferBegin, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, vertexBufferMemory);
}
