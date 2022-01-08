#pragma once
#include <GL/glew.h>
#include "common/vertex.h"

#include <iostream>
/* 
void glVertexAttribIPointer(	
GLuint index,   Specifies the index of shader location
GLint size,     Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4. 
GLenum type,    Specifies the data type of each component in the array. The initial value is GL_FLOAT.
normalized:     specifies whether fdata values should be normalized (GL_TRUE) (GL_FALSE) 
stride:         Specifies the byte offset between consecutive generic vertex attributes. The initial value is 0.
pointer:        Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of 
                        the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
);       

*/
class OpenglVertexBuffer
{
public:   
     struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;
    };
    OpenglVertexBuffer(){

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(position_attrib_index);
        glEnableVertexAttribArray(color_attrib_index); 
        glVertexAttribPointer(position_attrib_index, 2, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid*)vertex_offset );
        glVertexAttribPointer(color_attrib_index, 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid*)color_offset );


        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO 
        glBindVertexArray(0); 
    }

    ~OpenglVertexBuffer(){   
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw(){
        glBindVertexArray(VAO); 
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, 0);
    }

private:
    unsigned int VBO, VAO, EBO; 

    //shader location 0 1 2
    const unsigned int position_attrib_index{0};
    const unsigned int color_attrib_index{1}; 
    const unsigned int normal_attrib_index{2};

    const unsigned int  vertex_stride = sizeof(Vertex);

    GLintptr vertex_offset = 0 * sizeof(float);
    GLintptr color_offset = sizeof(glm::vec2);
    

   const std::vector<Vertex> vertices{
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    }; 

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
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
    }

    ~Shader(){glDeleteProgram(shaderProgram);}
    
    void use(){glUseProgram(shaderProgram);}

private:
   unsigned int shaderProgram;
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec3 aCol;\n"
        "out vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   ourColor = aCol;\n"
        "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
        "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
        "in vec3 ourColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(ourColor, 1.0f);\n"
        "}\n\0";

};

