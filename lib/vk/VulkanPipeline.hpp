#pragma once

#include "VulkanDevice.hpp"
// #include "VulkanSwapchain.hpp"
// #include "VulkanVertexBuffer.hpp"

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

    const std::string vertshader{"shaders/vert.spv"};
    const std::string fragshader{"shaders/frag.spv"};

};

