#pragma once

// lib
#include <glm/glm.hpp>
// std
#include <vector>


struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

typedef  uint32_t Index;

/* 
    Vulkan expects the data in your structure to be aligned in memory in a specific way, for example:

    Scalars have to be aligned by N (= 4 bytes given 32 bit floats).
    A vec2 must be aligned by 2N (= 8 bytes)
    A vec3 or vec4 must be aligned by 4N (= 16 bytes)
    A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
    A mat4 matrix must have the same alignment as a vec4.
 */

struct UniformBufferObject {
    alignas(16) glm::mat4 model{glm::mat4(1.0f)};
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

