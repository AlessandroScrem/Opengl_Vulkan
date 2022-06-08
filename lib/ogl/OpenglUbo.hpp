#pragma once

// lib
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// common
#include <vertex.h>

class OpenglUbo : public UniformBufferObject{
public:
    OpenglUbo(){
        glCreateBuffers(1, &ubo);
        glNamedBufferStorage(ubo, sizeof(UniformBufferObject), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

   void bind() {
        glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, ubo, offset, sizeof(UniformBufferObject));
        glNamedBufferSubData(ubo, offset, sizeof(UniformBufferObject), static_cast<UniformBufferObject*>(this));
    }

private:
    const int offset = 0;
    const int binding_point{0};
    unsigned int ubo{0};
};