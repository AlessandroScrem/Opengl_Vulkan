#pragma once
// common
#include <baseclass.hpp>
#include <vertex.h>
// lib
#include <GL/glew.h>
// std
#include <array>


/**
* @brief 
* @param GLuint  index location,  Specifies the index of shader location
* @param GLint   size,            Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4. 
* @param GLenum  type,            Specifies the data type of each component in the array. The initial value is GL_FLOAT.
* @param bool    normalized:      specifies whether fdata values should be normalized (GL_TRUE) (GL_FALSE) 
* @param GLuint  reloffset:       Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of 
                                       the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
*/
struct VertexInputAttributeDescription {
    GLuint     location;
    GLint      size;
    GLenum     type;
    GLboolean  normalized;   
    GLuint     reloffset;  
};

class Model;
class OpenglVertexBuffer;

class OpenglObjectBuilder : public ObjectBuilder{
private:
    std::unique_ptr<OpenglVertexBuffer> renderobject;
public:

    ObjectBuilder& Reset() override;
    virtual std::unique_ptr<RenderObject> build(Model &model, std::string shader) override;

};


class OpenglVertexBuffer : public RenderObject
{
public:   
    static std::array<VertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VertexInputAttributeDescription, 4> attributeDescriptions{};
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].size = 3;
        attributeDescriptions[0].type = GL_FLOAT;
        attributeDescriptions[0].normalized = GL_FALSE;
        attributeDescriptions[0].reloffset = (GLuint)offsetof(Vertex, pos);

        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].size = 3;
        attributeDescriptions[1].type = GL_FLOAT;
        attributeDescriptions[1].normalized = GL_FALSE;
        attributeDescriptions[1].reloffset = (GLuint)offsetof(Vertex, color);

        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].size = 3;
        attributeDescriptions[2].type = GL_FLOAT;
        attributeDescriptions[2].normalized = GL_FALSE;
        attributeDescriptions[2].reloffset = (GLuint)offsetof(Vertex, normal);

        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].size = 2;
        attributeDescriptions[3].type = GL_FLOAT;
        attributeDescriptions[3].normalized = GL_FALSE;
        attributeDescriptions[3].reloffset = (GLuint)offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    OpenglVertexBuffer() = default;
    ~OpenglVertexBuffer();

    void build(Model &model);
    void draw(GLenum mode);

private:

    void setVertexAttribPointer();
    bool prepared = false;
    
    GLuint VBO, VAO, IBO;
    const GLuint bindingIndex = 0;
    GLsizei _stride;
    GLsizei _indices_size;
};

