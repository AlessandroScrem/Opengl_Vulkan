#pragma once
// lib common
#include <baseclass.hpp>
#include <glsl_constants.h>
// std
#include <vector>
#include <array>
#include <map>

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

class VulkanShader;

struct ImageBindings{
    std::unique_ptr<VulkanImage> image;
    uint32_t    binding = 0;
};

struct UboBindings{
    std::unique_ptr<VulkanUbo>   ubo;
    uint32_t    binding = 0;
};


class VulkanShaderBuilder : public ShaderBuilder{
private:
    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    std::unique_ptr<VulkanShader> shader;
public:

    VulkanShaderBuilder(VulkanDevice &device, VulkanSwapchain &swapchain);

    virtual ShaderBuilder& Reset()override;
    virtual ShaderBuilder& type(GLSL::ShaderType id)  override;
    virtual ShaderBuilder& addTexture(std::string image, uint32_t binding)  override;
    virtual ShaderBuilder& setPolygonMode(GLSL::PolygonMode mode) override;
    virtual std::unique_ptr<Shader> build() override;

};

class VulkanShader : public Shader
{ 
struct ShaderBindigs{
    std::map<uint32_t, std::unique_ptr<VulkanImage> > imageBindings;
    std::map<uint32_t, std::unique_ptr<VulkanUbo> >   uboBindings; 

}shaderBindings;

public:
    friend class VulkanShaderBuilder;

    VulkanShader(VulkanDevice &device, VulkanSwapchain &swapchain, GLSL::ShaderType type = GLSL::PHONG);
    ~VulkanShader();

    VkPipeline getGraphicsPipeline(){ return graphicsPipeline;}
    const VkPipelineLayout & getPipelineLayout() const { return pipelineLayout; }
    void updateUbo(UniformBufferObject & mpv);
    void bind(VkCommandBuffer cmd);

private:

    void buildShaders();  
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void buid();
    void cleanupDescriptorPool();
    void cleanupPipeline();
    void createPipeline();

    // ----------------------------
    void createGlobalUbo();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();


    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

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
    bool prepared = false;

    const uint32_t globalUboBinding = 0;

    VkShaderModule vertModule;
    VkShaderModule fragModule;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
};
