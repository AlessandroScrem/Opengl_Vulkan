#pragma once
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

// lib
#include <glm/glm.hpp>

// std
#include <array>

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

 static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(VulkanDevice &device, VulkanSwapchain &swapchain);
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

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    }; 

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    }; 
};

