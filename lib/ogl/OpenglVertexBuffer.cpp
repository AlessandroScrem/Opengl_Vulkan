#include "OpenglVertexBuffer.hpp"
// common
#include <model.hpp>

OpenglVertexBuffer::OpenglVertexBuffer(Model &model)
{
    _indices_size       = static_cast<GLsizei>(model.indicesSize());
    auto vertices_size  = model.verticesSize();
    auto vertices_data  = model.verticesData();
    auto indices_data   = model.indicesData();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size * sizeof(Vertex), vertices_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices_size * sizeof(Index), indices_data, GL_STATIC_DRAW);
    
    setVertexAttribPointer();

    // You can unbind the buffers 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

OpenglVertexBuffer::~OpenglVertexBuffer()
{   
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void OpenglVertexBuffer::draw(GLenum mode)
{
    glBindVertexArray(VAO); 
    glDrawElements(mode, (GLsizei) _indices_size , GL_UNSIGNED_INT, 0);
}


void OpenglVertexBuffer::setVertexAttribPointer(){
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