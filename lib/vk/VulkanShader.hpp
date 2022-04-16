#pragma once
#include "VulkanDevice.hpp"
// lib common
#include <mytypes.hpp>
#include <glsl_constants.h>
// lib
#include <glm/glm.hpp>
// std
#include <string>
#include <vector>

class VulkanShader
{    
private:
    enum class ShaderSourceType{
        VertexShader,
        FragmentShader
    };

    class ShaderSource{
    public:
        ShaderSource(VulkanDevice &device, GLSL::ShaderType shaderType, ShaderSourceType type )
            : device{device}
            , type{type}
        { 
            create(shaderType);
            SPDLOG_TRACE("{} shader constructor", shadername); 
        }

        ~ShaderSource(){
            SPDLOG_TRACE("{} shader destructor", shadername); 
            vkDestroyShaderModule(device.getDevice(), shaderModule, nullptr);
        }

        VkPipelineShaderStageCreateInfo get()  {return vertShaderStageInfo;}

    private:
        VkShaderModule createShaderModule(const std::vector<char>& code);
        void create( GLSL::ShaderType shadertype);
        void create_from_spv(GLSL::ShaderType shaderType);
        void create_from_glsl(GLSL::ShaderType shaderType);

        VulkanDevice &device;

        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};

        std::string shadername{};
        ShaderSourceType type;
        bool precompiled = true;
    };

public:
    VulkanShader(VulkanDevice &device, GLSL::ShaderType type = GLSL::PHONG)
        : device{device}
        , shaderType{type} 
    {
        SPDLOG_TRACE("VulkanShader constructor"); 
        buildShaders();
    }

    ~VulkanShader(){
        SPDLOG_TRACE("VulkanShader destructor"); 
    }

    uint32_t size() {return static_cast<uint32_t>( shaderStages.size() );}
    VkPipelineShaderStageCreateInfo* data() {return shaderStages.data();}
    std::vector<VkPipelineShaderStageCreateInfo>  * get() {return  &shaderStages;}
   
private:

    void buildShaders()
    { 
        shaderStages.push_back(vertex_shader.get());
        shaderStages.push_back(fragment_shader.get());
    }

    VulkanDevice &device;

    GLSL::ShaderType shaderType;

    ShaderSource vertex_shader = ShaderSource(device, shaderType, ShaderSourceType::VertexShader);
    ShaderSource fragment_shader = ShaderSource(device, shaderType, ShaderSourceType::FragmentShader);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

};
