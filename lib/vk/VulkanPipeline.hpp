#pragma once

#include "VulkanDevice.hpp"


class VulkanPipeline
{
public:
    VulkanPipeline(VulkanDevice &device);
    ~VulkanPipeline();

    void createPipeline();
private: 

    VulkanDevice &device;

    VkShaderModule createShaderModule(const std::vector<char>& code);

    const std::string vertshader{"shaders/vert.spv"};
    const std::string fragshader{"shaders/frag.spv"};

};

