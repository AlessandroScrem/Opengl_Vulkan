#pragma once

#include "vertex.h"

// std
#include <string>
#include <vector>

class Model
{ 
public:
    Model(){ load(); }
    ~Model(){}

    void load();

    size_t verticesSize() const  {return vertices.size(); }
    size_t indicesSize() const   {return indices.size(); }

    const Vertex* verticesData() const {return vertices.data(); }
    const uint32_t* indicesData()  const {return indices.data(); }


private:

    std::vector<Vertex> vertices{};
    std::vector<Index> indices{};
    const std::string modelpath{"models/viking_room.obj"};
    //const std::string modelpath{"models/backpack/backpack.obj"};
};

