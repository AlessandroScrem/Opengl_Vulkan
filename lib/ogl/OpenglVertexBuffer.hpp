#pragma once
#include "common/vertex.h"
#include "common/model.hpp"
#include "common/Window.hpp"
#include "OpenglImage.hpp"
// lib
#include <GL/glew.h>
#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// std
#include <iostream>

/* 
void glVertexAttribIPointer(	
GLuint index location,  Specifies the index of shader location
GLint size,             Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4. 
GLenum type,            Specifies the data type of each component in the array. The initial value is GL_FLOAT.
bool normalized:        specifies whether fdata values should be normalized (GL_TRUE) (GL_FALSE) 
stride:                 Specifies the byte offset between consecutive generic vertex attributes. The initial value is 0.
pointer offset:         Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of 
);                          the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
*/
struct VertexInputAttributeDescription {
    GLint  location;
    GLint size;
    GLenum  type;
    GLboolean  normalized;  
    GLsizei  stride;  
    const void * offset;  
};

/*
    // Vector sample
    const std::vector<Vertex> vertices{
        // pos                  color               texCoord          
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, 
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, 
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, 
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, 

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
   };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };  
*/

class OpenglVertexBuffer
{
public:   
    static std::array<VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].size = 3;
        attributeDescriptions[0].type = GL_FLOAT;
        attributeDescriptions[0].normalized = GL_FALSE;
        attributeDescriptions[0].stride = sizeof(Vertex);
        attributeDescriptions[0].offset = (GLvoid*)offsetof(Vertex, pos);

        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].size = 3;
        attributeDescriptions[1].type = GL_FLOAT;
        attributeDescriptions[1].normalized = GL_FALSE;
        attributeDescriptions[1].stride = sizeof(Vertex);
        attributeDescriptions[1].offset = (GLvoid*)offsetof(Vertex, color);

        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].size = 2;
        attributeDescriptions[2].type = GL_FLOAT;
        attributeDescriptions[2].normalized = GL_FALSE;
        attributeDescriptions[2].stride = sizeof(Vertex);
        attributeDescriptions[2].offset = (GLvoid*)offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    OpenglVertexBuffer(Window &window, Model &model) : window{window}, model{model}
    {
        auto vertices_size = model.verticesSize();
        auto vertices_data = model.verticesData();
        auto indices_size  = model.indicesSize();
        auto indices_data  = model.indicesData();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices_size * sizeof(Vertex), vertices_data, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(Index), indices_data, GL_STATIC_DRAW);
        
        setVertexAttribPointer();

        // You can unbind the buffers 
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
    }

    ~OpenglVertexBuffer(){   
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw(){
        glBindVertexArray(VAO); 
        glBindTexture(GL_TEXTURE_2D, texture.getId());
        glDrawElements(GL_TRIANGLES, (GLsizei) model.indicesSize() , GL_UNSIGNED_INT, 0);
        glActiveTexture(GL_TEXTURE0);
    }

private:

    void setVertexAttribPointer(){
        auto attributes =  OpenglVertexBuffer::getAttributeDescriptions();
        for( const auto & attribute : attributes){
            glEnableVertexAttribArray(attribute.location); 
            glVertexAttribPointer(
                attribute.location, 
                attribute.size, 
                attribute.type, 
                attribute.normalized, 
                attribute.stride, 
                attribute.offset 
            );
        }
    }

    Window &window;
    OpenglImage texture{"textures/viking_room.png"};
    //OpenglImage texture{"textures/backpack-diffuse.png"};
    
    unsigned int VBO, VAO, EBO;

    Model &model; 

};

