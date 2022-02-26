#pragma once

#include "VulkanDevice.hpp"

class VulkanSwapchain;
class VulkanVertexBuffer;

class VulkanPipeline
{
public:
    VulkanPipeline(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanVertexBuffer &vertexbuffer);
    ~VulkanPipeline();


    // used by VulkanCommandBuffer
    VkPipeline getGraphicsPipeline(){ return graphicsPipeline;}
    
    // used by VulkanEngine
    const VkPipelineLayout & getPipelineLayout() const { return pipelineLayout; }
    void cleanupPipeline();
    void createPipeline();
    

private: 

    VkShaderModule createShaderModule(const std::vector<char>& code);

    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    VulkanVertexBuffer &vertexbuffer;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    const std::string vertshader{"data/shaders/vert.spv"};
    const std::string fragshader{"data/shaders/frag.spv"};
    
    
    // Opengl compatible Viewport (SashaWillems)
    // require: VK_KHR_MAINTENANCE1_EXTENSION_NAME extension support for passing negative viewport heights: 
    // FIXME  tentativo di uniformare opengl a vulkan  
    const bool Opengl_compatible_viewport = false; 
};

