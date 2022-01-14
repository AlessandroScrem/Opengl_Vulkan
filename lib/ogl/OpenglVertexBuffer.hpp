#pragma once
#include "common/vertex.h"
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


struct VertexInputAttributeDescription {
    GLint  location;
    GLint size;
    GLenum  type;
    GLboolean  normalized;  
    GLsizei  stride;  
    const void * offset;  
};

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
class OpenglVertexBuffer
{
public:   
    struct UniformBufferObject {
        alignas(16) glm::mat4 model{glm::mat4(1.0f)};
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
     struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

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
    };
    OpenglVertexBuffer(Window &window) : window{window}
    {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
        
        setVertexAttribPointer();

        // You can unbind the buffers 
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        createUniformBuffers();
    }

    ~OpenglVertexBuffer(){   
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw(){
        glBindVertexArray(VAO); 
        glBindTexture(GL_TEXTURE_2D, texture.getId());
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, 0);
        glActiveTexture(GL_TEXTURE0);
    }

    void updateUniformBuffers(){
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
      
        //ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //ubo.model = glm::scale(ubo.model, glm::vec3{2.0});
        //ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //ubo.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        auto [width, height] = window.GetWindowExtents();
        ubo.proj = glm::perspective(glm::radians(45.0f), width / (float) height, 0.1f, 10.0f);
        //ubo.proj[1][1] *= -1;
    }

    const UniformBufferObject & getUbo() const { return ubo; }

    void bindUniformBuffers() {
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(ubo.model) ); 
        glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(ubo.view) );
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(ubo.proj) );
        glBindBuffer(GL_UNIFORM_BUFFER, 0);  
    }

private:
    void    createUniformBuffers(){
        glGenBuffers(1, &uboMatrices);
        
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 3 * sizeof(glm::mat4));
    }

    void setVertexAttribPointer(){
        auto attributes =  Vertex::getAttributeDescriptions();
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
    OpenglImage texture{"textures/texture.jpg"};

    UniformBufferObject ubo{};

    unsigned int uboMatrices;
    
    unsigned int VBO, VAO, EBO; 


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
};

class Shader
{   
public:
    Shader(){
        // build and compile our shader program
        // ------------------------------------
        // vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        createUniformBlockBinding();
    }

    ~Shader(){glDeleteProgram(shaderProgram);}
    
    void use(){glUseProgram(shaderProgram);}
    
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    void createUniformBlockBinding(){
        uniformBlockIndex    = glGetUniformBlockIndex(shaderProgram, "ubo");        
        glUniformBlockBinding(shaderProgram,    uniformBlockIndex, 0);
    }

    unsigned int shaderProgram;
    unsigned int uniformBlockIndex;

    const char *vertexShaderSource = "#version 450 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aCol;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"
        "//struct UniformBufferObject {\n"
        "layout (std140) uniform UniformBufferObject {\n"
        "   mat4 model;\n"
        "   mat4 view;\n"
        "   mat4 proj;\n"
        "}ubo;\n"
        "out vec3 ourColor;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "   ourColor = aCol;\n"
        "   gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 1.0);\n"
        "   TexCoord = aTexCoord;\n"
        "}\0";

    const char *fragmentShaderSource = "#version 450 core\n"
        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(ourTexture, TexCoord);\n"
        "}\n\0";

};

