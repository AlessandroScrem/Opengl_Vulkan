#pragma once

// lib
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// common
#include <vertex.h>

struct OpenglUbo : UniformBufferObject{

    OpenglUbo(){
        glGenBuffers(1, &ubo);
    }

   void bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferObject),glm::value_ptr(model), GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);       
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, sizeof(UniformBufferObject));
        // unbind buffe
        glBindBuffer(GL_UNIFORM_BUFFER, 0);  
    }

private:
    const int binding_point = 0;
    unsigned int ubo;
};