#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"


class VulkanPipeline
{
public:
    VulkanPipeline(VulkanDevice &device, VulkanSwapchain &swapchian);
    ~VulkanPipeline();

    void createPipeline();

    // used by VulkanCommandBuffer
    VkPipeline getGraphicsPipeline(){ return graphicsPipeline;}
    

private: 

    VkShaderModule createShaderModule(const std::vector<char>& code);

    VulkanDevice &device;
    VulkanSwapchain &swapchian;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    const std::string vertshader{"shaders/vert.spv"};
    const std::string fragshader{"shaders/frag.spv"};

};

