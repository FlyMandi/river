#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};

extern const std::vector<Vertex> vertices;
