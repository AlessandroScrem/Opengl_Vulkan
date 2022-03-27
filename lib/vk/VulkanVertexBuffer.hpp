#pragma once
#include "VulkanDevice.hpp"
#include "VulkanUbo.hpp"
#include "VulkanImage.hpp"
//common lib
#include <vertex.h>
#include <model.hpp>
// lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// std
#include <array>

class VulkanDevice;
class VulkanSwapchain;

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanUbo &ubo, VulkanImage &vulkanimage, Model &model);
    ~VulkanVertexBuffer();

    std::array<VkVertexInputBindingDescription, 1> getBindingDescription() {
        std::array<VkVertexInputBindingDescription, 1> bindingDescription{};
        bindingDescription[0].binding = 0;
        bindingDescription[0].stride = sizeof(Vertex);
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
    std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);
        
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    VkBuffer getVertexBuffer() { return vertexBuffer._buffer; }
    VkBuffer getIndexBuffer() { return indexBuffer._buffer; }

    size_t getIndexSize() { return model.indicesSize(); }

    const VkDescriptorSetLayout &  getDescriptorSetLayout() const { return descriptorSetLayout; }
    const VkDescriptorSet & getDescriptorSet(size_t index) const { return descriptorSets[index]; }

    void createDescriptorPool();
    void createDescriptorSets();
    void cleanupDescriptorPool();

private:

    void createVertexBuffer();
    void createIndexBuffer();

    void createDescriptorSetLayout();


    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    VulkanUbo &ubo;
    VulkanImage &vulkanimage;

    AllocatedBuffer  vertexBuffer;
    AllocatedBuffer  indexBuffer;

    Model &model;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;



    //                  Coordinate system:
    //
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
    //  
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
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // 3  top right

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    }; 

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    }; 
 */
};

