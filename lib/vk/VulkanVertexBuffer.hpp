#pragma once
#include <baseclass.hpp>

    //                  Coordinate system:
    //     Vulkan viewport                 Opengl viewport
    // -----------------------      ---------------------
    // |                      |     |                    |
    // |         ^            |     |                    |
    // |         | y          |     |                    |
    // |         |            |     |                    |
    // |         o------->    |     |         o------>   |
    // |              x       |     |         |       x  |       
    // |                      |     |         | y        |
    // |                      |     |         v          |
    // |----------------------      |---------------------                              
    // 
    //   texture coord:                    indices 
    //  0,0                         3                   2
    //  ---------------------       ----------------------
    //  |                   |       |                    |
    //  |                   |       |                    |
    //  |                   |       |       origin       |
    //  |                   |       |        0.0         |
    //  |                   |       |                    |
    //  |                   |       |                    |
    //  |                   |       |                    |
    //  |___________________|       |____________________|
    //                     1,1      0                    1                                
/*
     const std::vector<Vertex> vertices = {
        // pos                  color               texCoord          
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 0  bottom left
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // 1  bottom right
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // 2  top right
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // 3  top left
    }; 
    const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0 }; 
*/

class VulkanDevice;
class Model;

class VulkanVertexBuffer : public RenderObject
{
public:
    VulkanVertexBuffer(VulkanDevice &device, Model &model);
    ~VulkanVertexBuffer();

    VkBuffer getVertexBuffer() { return vertexBuffer._buffer; }
    VkBuffer getIndexBuffer() { return indexBuffer._buffer; }

    size_t getIndexSize() { return indices_size; }

    void bind(VkCommandBuffer cmd, uint32_t imgeIndex);

private:

    void createVertexBuffer(Model &model);
    void createIndexBuffer(Model &model);

    VulkanDevice &device;
    bool prepared = false;

    size_t indices_size;

    AllocatedBuffer  vertexBuffer;
    AllocatedBuffer  indexBuffer;

};

