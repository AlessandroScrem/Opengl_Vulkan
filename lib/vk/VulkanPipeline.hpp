#pragma once
#include "VulkanDevice.hpp"

class VulkanSwapchain;
class VulkanVertexBuffer;
class VulkanShader;

class PipelineBuilder {
public:

	std::vector<VkPipelineShaderStageCreateInfo> *_shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo _depthStencil;

	VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
};


class VulkanPipeline
{
public:
    VulkanPipeline(VulkanDevice &device, 
            VulkanSwapchain &swapchain, 
            VulkanVertexBuffer &vertexbuffer,
            VulkanShader &vulkanshader);
    ~VulkanPipeline();


    // used by VulkanCommandBuffer
    VkPipeline getGraphicsPipeline(){ return graphicsPipeline;}
    
    // used by VulkanEngine
    const VkPipelineLayout & getPipelineLayout() const { return pipelineLayout; }
    void cleanupPipeline();
    void createPipeline();
    
private: 

    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    VulkanVertexBuffer &vertexbuffer;
    VulkanShader &vulkanshader;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;  
    
    // Opengl compatible Viewport (SashaWillems)
    // require: VK_KHR_MAINTENANCE1_EXTENSION_NAME extension support for passing negative viewport heights: 
    // FIXME  tentativo di uniformare opengl a vulkan  
    const bool Opengl_compatible_viewport = false; 
};

