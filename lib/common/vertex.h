#pragma once

// lib
#include <glm/glm.hpp>

typedef  uint32_t Index;
struct Vertex {
    glm::vec3 pos{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 texCoord{};

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && normal == other.normal && texCoord == other.texCoord;
    }
};

//Vulkan expects structure to be aligned as multiple of 16.
struct UniformBufferObject {
    alignas(16) glm::mat4 view{glm::mat4(1.0f)};
    alignas(16) glm::mat4 proj{glm::mat4(1.0f)};
    alignas(16) glm::vec3 viewPos{0.0f};
    alignas(16) glm::vec3 drawLines{glm::vec3(0.0f)};
};
