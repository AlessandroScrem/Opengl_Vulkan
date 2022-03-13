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
        ShaderSource(VulkanDevice &device, const char *source , ShaderSourceType type )
            : device{device}
            , source{source}
            , type{type}
        { 
            create();
            SPDLOG_TRACE("{} shader constructor", shadername); 
        }

        ~ShaderSource(){
            SPDLOG_TRACE("{} shader destructor", shadername); 
            vkDestroyShaderModule(device.getDevice(), shaderModule, nullptr);
        }

        VkPipelineShaderStageCreateInfo get()  {return vertShaderStageInfo;}

    private:
        VkShaderModule createShaderModule(const std::vector<uint32_t>& code);
        void create();

        VulkanDevice &device;

        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};

        const char *source;
        std::string shadername{};
        ShaderSourceType type;
    };

public:
    VulkanShader(VulkanDevice &device, ShaderType type = GLSL::TEXTURE_SHADER)
        : device{device}
        , shaderType{type} 
    {
        SPDLOG_TRACE("constructor"); 
        buildShaders();
    }

    ~VulkanShader(){
        SPDLOG_TRACE("destructor"); 
    }

    uint32_t size() {return static_cast<uint32_t>( shaderStages.size() );}
    VkPipelineShaderStageCreateInfo* data() {return shaderStages.data();}
    
private:

    void buildShaders()
    {  
        shaderStages.push_back(vertex_shader.get());
        shaderStages.push_back(fragment_shader.get());
    }

    VulkanDevice &device;

    ShaderType shaderType;
    ShaderSource vertex_shader = ShaderSource(device, shaderType.vshader, ShaderSourceType::VertexShader);
    ShaderSource fragment_shader = ShaderSource(device, shaderType.fshader, ShaderSourceType::FragmentShader);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

};
