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


class VulkanShaderBuilder : public ShaderBuilder{
private:
    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    VkDescriptorSetLayout* dsLayout;
    std::unique_ptr<VulkanShader> shader;
public:

    VulkanShaderBuilder(VulkanDevice &device, VulkanSwapchain &swapchain, VkDescriptorSetLayout* dslayout);

    virtual ShaderBuilder& Reset()override;
    virtual ShaderBuilder& type(GLSL::ShaderType id)  override;
    virtual ShaderBuilder& addTexture(std::string image, uint32_t binding)  override;
    virtual ShaderBuilder& setPolygonMode(GLSL::PolygonMode mode) override;
    virtual std::unique_ptr<Shader> build() override;

};

class VulkanShader : public Shader
{ 
public:
    friend class VulkanShaderBuilder;

    VulkanShader(VulkanDevice &device, VulkanSwapchain &swapchain, GLSL::ShaderType type = GLSL::PHONG);
    ~VulkanShader();

    void bind(VkCommandBuffer cmd, GLSL::PolygonMode mode, VkDescriptorSet* descriptorSet, uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets);

private:

    void buildShaders();  
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void buid();

    void createPipelineLayout();
    void createPipeline(GLSL::PolygonMode mode);

    void cleanupPipeline();

    // ----------------------------
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkDescriptorSetLayout* descriptorSetLayout;
    // VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL; 
    VkPipelineLayout pipelineLayout;
    std::array<VkPipeline, 2> graphicsPipeline;  

    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    GLSL::ShaderType shaderType;

    const bool Opengl_compatible_viewport = false; 
    bool prepared = false;

    VkShaderModule vertModule;
    VkShaderModule fragModule;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
};
