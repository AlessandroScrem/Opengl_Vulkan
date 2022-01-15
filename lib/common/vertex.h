#pragma once

// lib
#include <glm/glm.hpp>
// std
#include <vector>


struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};

typedef  uint32_t Index;

struct UniformBufferObject {
    alignas(16) glm::mat4 model{glm::mat4(1.0f)};
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

