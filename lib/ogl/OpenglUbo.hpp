#pragma once

// lib
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// common
#include <vertex.h>

class OpenglUbo {
public:
    OpenglUbo(GLsizeiptr size, GLuint binding, const void *data) : bufferSize{size}, binding_point{binding}, mapped{data}
    {
        glCreateBuffers(1, &ubo);
        glNamedBufferStorage(ubo, bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

   void bind() {
        bind(mapped, bufferSize);
    }

   void bind(const void *data, GLsizeiptr size) {
        glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, ubo, offset, size);
        glNamedBufferSubData(ubo, offset, size, data);
    }

private:
    GLintptr   offset{0};
    GLuint     binding_point{0};
    GLsizeiptr bufferSize{0};
    const void *mapped = nullptr;
    GLuint     ubo;
};