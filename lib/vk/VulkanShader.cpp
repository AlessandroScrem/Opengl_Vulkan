#include "VulkanShader.hpp"
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

void VulkanShader::ShaderSource::create_from_glsl(GLSL::ShaderType shaderType)
{ 
    std::vector<char> glsl;
    std::vector<char> spv;

    if(type == ShaderSourceType::VertexShader){
        auto glsl = GLSL::readFile(GLSL::getname(shaderType) + ".vert");
        auto glsl_str = std::string(begin(glsl), end(glsl));
        spv = compileGlslToSvp(glsl_str, shaderc_glsl_vertex_shader);
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shadername = "VERTEX";
    }
    
    if(type == ShaderSourceType::FragmentShader){
        auto glsl = GLSL::readFile(GLSL::getname(shaderType) + ".frag");
        auto glsl_str = std::string(begin(glsl), end(glsl));
        spv = compileGlslToSvp(glsl_str, shaderc_glsl_fragment_shader);
        vertShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;   
        shadername = "FRAGMENT";
    }

    shaderModule = createShaderModule(spv);

    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "main";
}

void VulkanShader::ShaderSource::create_from_spv(GLSL::ShaderType shaderType)
{ 
    std::vector<char> spv;
    if(type == ShaderSourceType::VertexShader){
        spv = GLSL::readFile(GLSL::getname(shaderType) + ".vert.spv");
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shadername = "VERTEX";
    }
    
    if(type == ShaderSourceType::FragmentShader){
        spv = GLSL::readFile(GLSL::getname(shaderType) + ".frag.spv");
        vertShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shadername = "FRAGMENT";
    }

    shaderModule = createShaderModule(spv);

    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "main";
}

VkShaderModule VulkanShader::ShaderSource::createShaderModule(const std::vector<char>& code) {
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

void VulkanShader::ShaderSource::create( GLSL::ShaderType shaderType)
{
    if(precompiled){
        create_from_spv(shaderType);
    }else{
        create_from_glsl(shaderType);
    }
}
