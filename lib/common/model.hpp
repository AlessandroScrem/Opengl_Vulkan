#pragma once

#include "vertex.h"
// std
#include <vector>

constexpr char  defmodel[] = "data/models/viking_room.obj";

class Model
{ 
public:
    enum class UP{
        YUP,
        ZUP
    };

    Model(const char * modelpath  = defmodel, UP up = UP::YUP );
    ~Model();

    void load(const char * modelpath);

    size_t verticesSize() const  {return vertices.size(); }
    size_t indicesSize() const   {return indices.size(); }

    const Vertex* verticesData() const {return vertices.data(); }
    const uint32_t* indicesData()  const {return indices.data(); }

    void set_transform(glm::mat4 t)  {transform = transform * t ; }
    const glm::mat4 get_tranform() const {return transform; }


private:

    std::vector<Vertex> vertices{};
    std::vector<Index> indices{};
    glm::mat4 transform = glm::mat4(1.0);
};

