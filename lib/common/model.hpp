#pragma once

#include "vertex.h"
// std
#include <vector>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp> 

struct Transformations
{
    glm::vec3 T{};
    glm::vec3 R{};
    glm::vec3 S{1.0f};
};

struct Node 
{

    void set(const Transformations &tra){
        this->Tra = tra;
    }
    Transformations get (){
        return Tra;
    }

    /**
     * @brief Get final  matrix  
     * 
     * @return glm::mat4 
     */
    glm::mat4 getfinal() { return  local() * upMatrix;}

    void set_upperMatrix(glm::mat4 mat) {upMatrix = mat;}

private:
    /**
     * @brief Calculate local tranformations
     * 
     */
    glm::mat4 local() {
        // X = pitch Y = yaw Z = roll          
        glm::mat4 rot = glm::yawPitchRoll(glm::radians(Tra.R.y), glm::radians(Tra.R.x), glm::radians(Tra.R.z));
        glm::mat4 trasl = glm::translate(glm::mat4(1.0f), Tra.T);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), Tra.S);
        return  trasl * rot * scale;
    }

    Transformations Tra{};

    //TODO  represent upper node transformation
    glm::mat4 upMatrix{glm::mat4(1.0f)};
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

    Node node{};
    
private:

    void init_tranform(UP up);

    std::vector<Vertex> vertices{};
    std::vector<Index> indices{};
};

