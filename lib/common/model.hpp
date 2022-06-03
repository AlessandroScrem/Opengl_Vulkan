#pragma once

#include "vertex.h"
// std
#include <vector>
#include <glm/gtx/euler_angles.hpp>

struct Node
{
    glm::vec3 T{};
    glm::vec3 R{};
    glm::vec3 S{1.0, 1.0, 1.0};

    glm::mat4 upMatrix{glm::mat4(1.0f)};
    glm::mat4 matrix{glm::mat4(1.0f)};

    void rotate(glm::vec3 r) { R = r; apply(*this);}
    void translate(glm::vec3 r) { T = r; apply(*this);}
    void scale(glm::vec3 s) { S = s; apply(*this);}

    glm::mat4 apply() {return  apply(*this) * upMatrix;}
private:
    glm::mat4& apply(const Node &node) {
        T = node.T; R = node.R; S = node.S;
        // X = pitch Y = yaw Z = roll          
        glm::mat4 rot = glm::yawPitchRoll(glm::radians(R.y), glm::radians(R.x), glm::radians(R.z));
        glm::mat4 trasl = glm::translate(glm::mat4(1.0f), T);
        matrix = trasl * rot;
        return matrix;
    }
};

class Model
{ 
public:
    enum class UP{
        YUP,
        ZUP
    };
    
    Model( UP up = UP::YUP );
    Model(const char * modelpath, UP up = UP::YUP );
    Model(const std::vector<Vertex> &vertices, std::vector<uint16_t> &indices,  UP up = UP::YUP );
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

