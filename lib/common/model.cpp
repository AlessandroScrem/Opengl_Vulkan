#include "model.hpp"
#include "mytypes.hpp"
// lib
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
// std
#include <unordered_map>

constexpr char  defmodel[] = "data/models/viking_room.obj";

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}  

Model::Model(const char * modelpath /*  = defmodel */, UP up /* = UP::YUP */ ) 
{    

    init_tranform(up);
    load(modelpath); 
}

Model::~Model(){}

void Model::init_tranform(UP up){
   if(up == UP::ZUP) 
    {   
        // rotate model to y up
        node.set_upperMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    }    
}

void Model::load(const char *modelpath)
{
    if(!modelpath){
        return;
    }

    spdlog::info("loading {} ... ", modelpath);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelpath)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, Index> uniqueVertices{};

    SPDLOG_DEBUG("size_of shapes = {}", shapes.size());   

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }
            
            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }


            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<Index>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }

    if(vertices.size() == 0 || indices.size() == 0){
        throw std::runtime_error("failed to load model, vertices size !");
    }

    SPDLOG_INFO("size_of Vertices = {}", sizeof(Vertex) * vertices.size());   
    SPDLOG_INFO("Vertices.size() = {}", vertices.size());   
    SPDLOG_INFO("Indices.size()  = {}", indices.size()); 
   
}

Model& Model::axis()
{
    static Model axis{};
    // Setup vertices
    axis.vertices =
    {
        { {  0.0f,  0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }},
        { {  1.0f,  0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }},

        { {  0.0f,  0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }},
        { {  0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }},

        { {  0.0f,  0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }},
        { {  0.0f,  0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }}
    };
    // Setup indices
    axis.indices = { 0, 1, 2, 3, 4, 5};

    return axis;
}
