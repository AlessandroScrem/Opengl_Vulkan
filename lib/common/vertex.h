#pragma once

// lib
#include <glm/glm.hpp>
// std
#include <vector>

// struct UniformBufferObject {
//     alignas(16) glm::mat4 model;
//     alignas(16) glm::mat4 view;
//     alignas(16) glm::mat4 proj;
// };


struct MyVertex {
    glm::vec2 pos;
    glm::vec3 color;
};

struct MyMesh {
    const std::vector<MyVertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    }; 

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };  
};