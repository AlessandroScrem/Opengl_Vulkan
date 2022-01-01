#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"


class VulkanPipeline
{
public:
    VulkanPipeline(VulkanDevice &device, VulkanSwapchain &swapchian);
    ~VulkanPipeline();

    void createPipeline();
private: 

    VulkanDevice &device;
    VulkanSwapchain &swapchian;

    VkPipelineLayout pipelineLayout;

    VkShaderModule createShaderModule(const std::vector<char>& code);

    const std::string vertshader{"shaders/vert.spv"};
    const std::string fragshader{"shaders/frag.spv"};

};

