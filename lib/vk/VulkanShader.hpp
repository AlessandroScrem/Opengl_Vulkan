#pragma once
#include "VulkanDevice.hpp"
// lib common
#include <mytypes.hpp>
// std
#include <string>
#include <vector>
#include <fstream>

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

struct ShaderType{
    const char * vshader;
    const char * fshader;
};
   
inline  constexpr ShaderType TEXTURE_SHADER{
    .vshader = "data/shaders/vert.spv",
    .fshader = "data/shaders/frag.spv"
};


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
            SPDLOG_TRACE("{} shader constructor", shadername); 
            create();
        }

        ~ShaderSource(){
            SPDLOG_TRACE("{} shader destructor", shadername); 
            vkDestroyShaderModule(device.getDevice(), shaderModule, nullptr);
        }

        VkPipelineShaderStageCreateInfo get()  {return vertShaderStageInfo;}

    private:
        VkShaderModule createShaderModule(const std::vector<char>& code) {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }
        return shaderModule;
        }

        void create(){
            shaderModule = createShaderModule(readFile(source));

            if(type == ShaderSourceType::VertexShader){
                vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                shadername = "VERTEX";
            }

            if(type == ShaderSourceType::FragmentShader){
                vertShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                shadername = "FRAGMENT";
            }
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.module = shaderModule;
            vertShaderStageInfo.pName = "main";
        }

        VulkanDevice &device;

        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};

        const char *source;
        ShaderSourceType type;
        std::string shadername; 
    };

public:
    VulkanShader(VulkanDevice &device, ShaderType type = TEXTURE_SHADER)
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
