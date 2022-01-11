#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"

// lib
#include <glm/glm.hpp>

// std
#include <array>

/* 
    Vulkan expects the data in your structure to be aligned in memory in a specific way, for example:

    Scalars have to be aligned by N (= 4 bytes given 32 bit floats).
    A vec2 must be aligned by 2N (= 8 bytes)
    A vec3 or vec4 must be aligned by 4N (= 16 bytes)
    A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
    A mat4 matrix must have the same alignment as a vec4.
 */
struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

 static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
};

class VulkanDevice;
class VulkanSwapchain;

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(VulkanDevice &device, VulkanSwapchain &swapchain,  VulkanImage &vulkanimage);
    ~VulkanVertexBuffer();

    VkBuffer getVertexBuffer() { return vertexBuffer; }
    VkBuffer getIndexBuffer() { return indexBuffer; }
    size_t getIndexSize() { return indices.size(); }

    void updateUniformBuffer(uint32_t currentImage);
    const VkDescriptorSetLayout &  getDescriptorSetLayout() const { return descriptorSetLayout; }
    const VkDescriptorSet & getDescriptorSet(size_t index) const { return descriptorSets[index]; }

    // recreated and cleaned from VulkanEngine.recreateswapchain()
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void cleanupDescriptorPool();
    void cleanupUniformBuffers();

private:
    void createVertexBuffer();
    void createIndexBuffer();

    void createDescriptorSetLayout();


    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    VulkanImage &vulkanimage;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    // texture coord:
    //  
    //  0,0
    //  ---------------------
    //  |                   |
    //  |                   |
    //  |                   |
    //  |                   |
    //  |                   |
    //  |                   |
    //  |                   |
    //  |___________________|
    //                     1,1 
    
    const std::vector<Vertex> vertices = {
        // pos           color                texCoord          
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f},   {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    }; 

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    }; 
};

