#pragma once

// lib
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// common
#include <vertex.h>

struct OpenglUbo : UniformBufferObject{

    OpenglUbo(){
        glGenBuffers(1, &uboMatrices);
        
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferObject), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, sizeof(UniformBufferObject));
    }
   void bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferObject),glm::value_ptr(model), GL_STATIC_DRAW);
        // glBufferSubData(GL_UNIFORM_BUFFER, offsetof(UniformBufferObject, model), sizeof(glm::mat4), glm::value_ptr(model) ); 
        // glBufferSubData(GL_UNIFORM_BUFFER, offsetof(UniformBufferObject, view),  sizeof(glm::mat4), glm::value_ptr(view) );
        // glBufferSubData(GL_UNIFORM_BUFFER, offsetof(UniformBufferObject, proj),  sizeof(glm::mat4), glm::value_ptr(proj) );
        glBindBuffer(GL_UNIFORM_BUFFER, 0);  
    }

private:
    unsigned int uboMatrices;
};