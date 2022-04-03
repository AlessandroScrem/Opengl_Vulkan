#include "VulkanShader.hpp"
// lib
#include <shaderc/shaderc.hpp>
// std
#include <string>

std::vector<uint32_t> compileGlslToSvp(const char *source, shaderc_shader_kind kind)
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
    return {svCompilationResult.cbegin(), svCompilationResult.cend()};
}

void VulkanShader::ShaderSource::create()
{
    std::vector<uint32_t> fragmentCodeCompiled;

    if(type == ShaderSourceType::VertexShader){
        fragmentCodeCompiled = compileGlslToSvp(source, shaderc_glsl_vertex_shader);
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shadername = "VERTEX";
    }

    if(type == ShaderSourceType::FragmentShader){
        fragmentCodeCompiled = compileGlslToSvp(source, shaderc_glsl_fragment_shader);
        vertShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shadername = "FRAGMENT";
    }

    shaderModule = createShaderModule(fragmentCodeCompiled);

    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "main";
}

VkShaderModule VulkanShader::ShaderSource::createShaderModule(const std::vector<uint32_t>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size()*sizeof(uint32_t);
    createInfo.pCode = code.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
return shaderModule;
}
