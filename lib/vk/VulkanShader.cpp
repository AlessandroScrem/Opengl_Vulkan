#include "VulkanShader.hpp"
#include "vk_initializers.h"
// lib
#include <shaderc/shaderc.hpp>
// std
#include <string>

std::vector<char> compileGlslToSvp(std::string &source, shaderc_shader_kind kind)
{
    const shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_size);

    const auto svCompilationResult = compiler.CompileGlslToSpv(source, kind, "shaderc_s", options);
    if (svCompilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
    {   
        std::string shadertype;
        switch (kind)
        {
        case shaderc_glsl_vertex_shader:
            shadertype = "shaderc_glsl_vertex_shader";
            break;
        
        case shaderc_glsl_fragment_shader:
            shadertype = "shaderc_glsl_fragment_shader";
            break;
        
        default:
            shadertype = "unknown";
            break;
        }

        spdlog::error("failed to compile shader {} {}", shadertype, svCompilationResult.GetErrorMessage());
        throw std::runtime_error("failed to compile shader ");
    }

    //convert vector<uin32_t> to vector<char> 
    std::vector<char> data;
    for (const auto s : svCompilationResult)
    { 
        char c[4];
        memcpy(c, (void*)&s, sizeof(uint32_t));
        for(int i = 0; i<sizeof(uint32_t); ++i)
            data.push_back(c[i]); 
    }

    return {data.cbegin(), data.cend()};
}

VulkanShader::~VulkanShader(){
    SPDLOG_TRACE("VulkanShader destructor"); 

    vkDestroyShaderModule(device.getDevice(), vertModule, nullptr);
    vkDestroyShaderModule(device.getDevice(), fragModule, nullptr);
}




VkShaderModule VulkanShader::createShaderModule(const std::vector<char>& code) {
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


void VulkanShader::buildShaders(){ 

    std::vector<char> vertshader_spv{};
    std::vector<char> fragshader_spv{};
    
    if(precompiled)
    {
        vertshader_spv = GLSL::readFile(GLSL::getname(shaderType) + ".vert.spv");
        fragshader_spv = GLSL::readFile(GLSL::getname(shaderType) + ".frag.spv");
    }else
    {
        auto glsl_vertshader = GLSL::readFile(GLSL::getname(shaderType) + ".vert");
        auto vsh_str = std::string(begin(glsl_vertshader), end(glsl_vertshader));
        auto vspv = compileGlslToSvp(vsh_str, shaderc_glsl_vertex_shader);
        vertshader_spv.assign(vspv.begin(), vspv.end());

        auto glsl_fragshader = GLSL::readFile(GLSL::getname(shaderType) + ".frag");
        auto frag_str = std::string(begin(glsl_fragshader), end(glsl_fragshader));
        auto fspv = compileGlslToSvp(frag_str, shaderc_glsl_fragment_shader);
        fragshader_spv.assign(fspv.begin(), fspv.end());
    }


    vertModule = createShaderModule(vertshader_spv);
    fragModule = createShaderModule(fragshader_spv);

    
    shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertModule));
        
    shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragModule));

}

