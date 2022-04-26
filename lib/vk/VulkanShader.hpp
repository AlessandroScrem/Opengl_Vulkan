#pragma once
#include "VulkanDevice.hpp"
// lib common
#include <mytypes.hpp>
#include <glsl_constants.h>
// std
#include <vector>

class VulkanShader
{    
public:
    VulkanShader(VulkanDevice &device, GLSL::ShaderType type = GLSL::PHONG)
        : device{device}
        , shaderType{type} 
    {
        SPDLOG_TRACE("VulkanShader constructor"); 
        buildShaders();
    }

    ~VulkanShader();

    uint32_t size() {return static_cast<uint32_t>( shaderStages.size() );}
    VkPipelineShaderStageCreateInfo* data() {return shaderStages.data();}
    std::vector<VkPipelineShaderStageCreateInfo>  * get() {return  &shaderStages;}
   
private:

    void buildShaders();  
    VkShaderModule createShaderModule(const std::vector<char>& code);

    VulkanDevice &device;

    GLSL::ShaderType shaderType;
    bool precompiled = true;

    VkShaderModule vertModule;
    VkShaderModule fragModule;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
};
