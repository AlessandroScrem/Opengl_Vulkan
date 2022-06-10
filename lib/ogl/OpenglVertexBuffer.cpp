#include "OpenglVertexBuffer.hpp"
// common
#include <model.hpp>


ObjectBuilder& OpenglObjectBuilder::Reset(){
    renderobject = std::make_unique<OpenglVertexBuffer>();
    return *this;
}

std::unique_ptr<RenderObject> 
OpenglObjectBuilder::build(Model &model, std::string shadername)
{
    renderobject->shader = shadername;
    renderobject->objNode = model.node;
    renderobject->build(model);
    std::unique_ptr<RenderObject> result = std::move(this->renderobject);
    return result;
}

void OpenglVertexBuffer::build(Model &model)
{
    _indices_size       = static_cast<GLsizei>(model.indicesSize());
    _stride             = sizeof(Vertex);
    auto vertices_size  = model.verticesSize();
    auto vertices_data  = model.verticesData();
    auto indices_data   = model.indicesData();

    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, vertices_size * sizeof(Vertex), vertices_data, GL_DYNAMIC_STORAGE_BIT);
    
    glCreateBuffers(1, &IBO);
    glNamedBufferStorage(IBO, _indices_size * sizeof(Index), indices_data, GL_DYNAMIC_STORAGE_BIT);

    glCreateVertexArrays(1, &VAO);

    //offset of the first element of the buffer
    const GLintptr offset = 0;
    glVertexArrayVertexBuffer(VAO, bindingIndex, VBO, offset, _stride);
    glVertexArrayElementBuffer(VAO, IBO);
    
    setVertexAttribPointer();

    prepared = true; 
}

OpenglVertexBuffer::~OpenglVertexBuffer()
{ 
    if (prepared){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &IBO);
    }  
}

void OpenglVertexBuffer::draw(GLenum mode)
{
    if(!prepared){
        return;
    }
    glBindVertexArray(VAO); 
    glDrawElements(mode, (GLsizei) _indices_size , GL_UNSIGNED_INT, 0);
}


void OpenglVertexBuffer::setVertexAttribPointer(){
    auto attributes =  OpenglVertexBuffer::getAttributeDescriptions();
    for( const auto & attribute : attributes){
        glEnableVertexArrayAttrib(VAO, attribute.location); 
        glVertexArrayAttribFormat( 
            VAO,
            attribute.location, 
            attribute.size, 
            attribute.type, 
            attribute.normalized, 
            attribute.reloffset 
        );
        glVertexArrayAttribBinding(VAO, attribute.location, bindingIndex);
    }
}