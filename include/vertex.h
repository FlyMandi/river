#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>

#include <vector>
#include <array>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};

extern const std::vector<Vertex> vertices;

extern VkVertexInputBindingDescription getVertexBindingDescription();
extern std::array<VkVertexInputAttributeDescription, 2> getVertexAttributeDescriptions();
