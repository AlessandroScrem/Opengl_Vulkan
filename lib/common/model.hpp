#pragma once

#include "vertex.h"
// std
#include <vector>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp> 


struct Node
{
    glm::vec3 T{};
    glm::vec3 R{};
    glm::vec3 S{1.0f};

    //TODO  represent upper node transformation
    glm::mat4 upMatrix{glm::mat4(1.0f)};

    void rotate(glm::vec3 r) { R = r; apply();}
    void translate(glm::vec3 r) { T = r; apply();}
    void scale(glm::vec3 s) { S = s; apply();}
    /**
     * @brief Get final  matrix  
     * 
     * @return glm::mat4 
     */
    glm::mat4 getfinal() { apply(); return  matrix * upMatrix;}
private:
    /**
     * @brief Calculate matrix from local tranformations
     * 
     */
    void apply() {
        // X = pitch Y = yaw Z = roll          
        glm::mat4 rot = glm::yawPitchRoll(glm::radians(R.y), glm::radians(R.x), glm::radians(R.z));
        glm::mat4 trasl = glm::translate(glm::mat4(1.0f), T);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), S);
        matrix = trasl * rot * scale;
    }

    // precalculated local matrix   
    glm::mat4 matrix{glm::mat4(1.0f)};
};

class Model
{ 
public:
    enum class UP{
        YUP,
        ZUP
    };
    
    Model(const char * modelpath = nullptr, UP up = UP::YUP );
    ~Model();

    void load(const char * modelpath);
    static Model& axis();

    size_t verticesSize() const  {return vertices.size(); }
    size_t indicesSize() const   {return indices.size(); }

    const Vertex* verticesData() const {return vertices.data(); }
    const uint32_t* indicesData()  const {return indices.data(); }

    void set_node(Node &t)  {node = t ; }
    Node & get_Node()  {return node; }


private:

    void init_tranform(UP up);

    std::vector<Vertex> vertices{};
    std::vector<Index> indices{};
    Node node{};
};

