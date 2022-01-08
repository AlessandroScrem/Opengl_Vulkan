#pragma once
#include <GL/glew.h>

#include <vector>
#include <glm/glm.hpp>

class Mesh
{
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
    };

    Mesh(){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);

        //position location 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        //color location 1
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)) );
        glEnableVertexAttribArray(1);
   
        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0); 
    }

    ~Mesh(){   
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void draw(){
        glBindVertexArray(VAO); 
        //glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()) );
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, 0);
    }
private:
    unsigned int VBO, VAO, EBO;
    const std::vector<Vertex> vertices{
            // positions          // colors           
            {{-0.5f, -0.5f, 0.0f},{1.0f, 0.0f, 0.0f}}, 
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
        };
    const std::vector<uint16_t> indices = {0, 1, 2};  
};

 