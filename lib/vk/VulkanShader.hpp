#pragma once
// lib common
#include <baseclass.hpp>
#include <glsl_constants.h>
// std
#include <vector>
#include <array>

static const std::array<VkVertexInputBindingDescription, 1> getBindingDescription() {
    std::array<VkVertexInputBindingDescription, 1> bindingDescription{};
    bindingDescription[0].binding = 0;
    bindingDescription[0].stride = sizeof(Vertex);
    bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}
static const std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
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
class VulkanDevice;
class VulkanSwapchain;
class VulkanUbo;
class VulkanImage;

class VulkanShader : public Shader
{ 
struct ShaderBindigs{
    std::unique_ptr<VulkanImage> image;
    std::unique_ptr<VulkanUbo>   ubo;
    uint32_t    imageBindig = 0;
    uint32_t    uboBindig = 0; 

}shaderBindings;

public:
    VulkanShader(VulkanDevice &device, VulkanSwapchain &swapchain, GLSL::ShaderType type = GLSL::PHONG);
    ~VulkanShader();

    const VkDescriptorSet & getDescriptorSet(size_t index) const { return descriptorSets[index]; }
    VkPipeline getGraphicsPipeline(){ return graphicsPipeline;}
    const VkPipelineLayout & getPipelineLayout() const { return pipelineLayout; }
    VulkanUbo & getUbo();
   
    void cleanupDescriptorPool();
    void cleanupPipeline();

    void addTexture(std::string imagepath, uint32_t binding);
    void addUbo(uint32_t binding);
    void addConstant(uint32_t binding);
    void setPolygonMode(VkPolygonMode mode);
    void setTopology(VkPrimitiveTopology mode);
    void buid();


private:

    VkShaderModule createShaderModule(const std::vector<char>& code);
    void buildShaders();  
    void createPipeline();

    // ----------------------------
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();


    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    // ----------------------------
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL; 
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;  

    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    GLSL::ShaderType shaderType;

    const bool Opengl_compatible_viewport = false; 
    bool precompiled = true;

    VkShaderModule vertModule;
    VkShaderModule fragModule;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

    std::vector<VkDescriptorPoolSize> shaderPoolsize{};
    std::vector<VkWriteDescriptorSet> descriptorSet{};
    std::vector<VkWriteDescriptorSet> shaderWrites{};
};
