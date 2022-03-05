#pragma once

#include "vertex.h"

// std
#include <vector>

constexpr char  defmodel[] = "data/models/viking_room.obj";

class Model
{ 
public:
    Model(const char * modelpath  = defmodel) { load(modelpath); }
    ~Model(){}

    void load(const char * modelpath);

    size_t verticesSize() const  {return vertices.size(); }
    size_t indicesSize() const   {return indices.size(); }

    const Vertex* verticesData() const {return vertices.data(); }
    const uint32_t* indicesData()  const {return indices.data(); }


private:

    std::vector<Vertex> vertices{};
    std::vector<Index> indices{};

};

