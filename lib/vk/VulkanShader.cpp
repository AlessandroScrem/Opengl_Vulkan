#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanUbo.hpp"
#include "VulkanImage.hpp"
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

VulkanShaderBuilder::VulkanShaderBuilder(VulkanDevice &device, VulkanSwapchain &swapchain)
: device{device}, swapchain{swapchain}
{
}

ShaderBuilder&  VulkanShaderBuilder::Reset(){
    this->shader = std::make_unique<VulkanShader>(device, swapchain);
    return *this;
}

ShaderBuilder& VulkanShaderBuilder::type(GLSL::ShaderType id) {
     this->shader->shaderType = id;
     return *this; 
}

ShaderBuilder& VulkanShaderBuilder::addTexture(std::string imagepath, uint32_t id ) {
    size_t swapchainImageSize = swapchain.getSwapchianImageSize();

    auto image =  std::make_unique<VulkanImage>(device, imagepath);

    this->shader->shaderBindings.imageBindings.emplace(id, std::move(image));
    return *this;
}

ShaderBuilder& VulkanShaderBuilder::setPolygonMode(uint32_t mode) {
    switch (mode)
    {
    case 0:
        this->shader->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        this->shader->polygonMode = VK_POLYGON_MODE_FILL;
        break;
    case 1:
        this->shader->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        this->shader->polygonMode = VK_POLYGON_MODE_LINE;
        break;   
    default:
        break;
    }
    return *this;
}

std::unique_ptr<Shader> VulkanShaderBuilder::build() {
    this->shader->buid();
    std::unique_ptr<Shader> result = std::move(this->shader);
    return result;
}

VulkanShader::VulkanShader(VulkanDevice &device, VulkanSwapchain &swapchain, GLSL::ShaderType type /* = GLSL::PHONG */)
    : device{device}
    , swapchain{swapchain}
    , shaderType{type} 
{
    SPDLOG_TRACE("VulkanShader constructor");
}

VulkanShader::~VulkanShader(){
    SPDLOG_TRACE("VulkanShader destructor");

    if(prepared){
        SPDLOG_DEBUG("destroy shader");

        cleanupPipeline();
        cleanupDescriptorPool();
        vkDestroyDescriptorSetLayout(device.getDevice(), descriptorSetLayout, nullptr);
        SPDLOG_TRACE("vkDestroyDescriptorSetLayout");

        vkDestroyShaderModule(device.getDevice(), vertModule, nullptr);
        vkDestroyShaderModule(device.getDevice(), fragModule, nullptr);

    } 
}

void VulkanShader::buid()
{
    SPDLOG_DEBUG("VulkanShader build");

    buildShaders();                 

    createGlobalUbo();
    createDescriptorSetLayout();    
    createDescriptorPool();        
    createDescriptorSets();         

    createPipeline(); 

    prepared = true;             
}

 void VulkanShader::bind(VkCommandBuffer cmd, uint32_t imageIndex)
 {
    if(!prepared){
        return;
    }

    for(auto& uboBinding : shaderBindings.uboBindings){
        uboBinding.second->bind(imageIndex);     
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, nullptr);
 }   

void VulkanShader::cleanupDescriptorPool()
{
    SPDLOG_TRACE("cleanupDescriptorPool");
    vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
}

void VulkanShader::cleanupPipeline()
{
    SPDLOG_TRACE("vkDestroyPipeline");
    vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
    SPDLOG_TRACE("vkDestroyPipelineLayout");
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);    
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
        vertshader_spv = GLSL::readFile(GLSL::getPath(shaderType) + ".vert.spv");
        fragshader_spv = GLSL::readFile(GLSL::getPath(shaderType) + ".frag.spv");
    }else
    {
        auto glsl_vertshader = GLSL::readFile(GLSL::getPath(shaderType) + ".vert");
        auto vsh_str = std::string(begin(glsl_vertshader), end(glsl_vertshader));
        auto vspv = compileGlslToSvp(vsh_str, shaderc_glsl_vertex_shader);
        vertshader_spv.assign(vspv.begin(), vspv.end());

        auto glsl_fragshader = GLSL::readFile(GLSL::getPath(shaderType) + ".frag");
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

void VulkanShader::createGlobalUbo() {
    size_t swapchainImageSize = swapchain.getSwapchianImageSize();
    auto ubo = std::make_unique<VulkanUbo>(device, swapchainImageSize); 
    shaderBindings.uboBindings.emplace(globalUboBinding, std::move(ubo) );
}


void VulkanShader::createDescriptorSetLayout()
{
    SPDLOG_TRACE("createDescriptorSetLayout");

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};

    for(auto& imageBinding : shaderBindings.imageBindings){
        auto samplerLayoutBinding = vkinit::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            imageBinding.first);
        layoutBindings.push_back(samplerLayoutBinding);
    }
    for(auto& uboBinding : shaderBindings.uboBindings){
        auto uboLayoutBinding =  vkinit::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_VERTEX_BIT,
            uboBinding.first);
        layoutBindings.push_back(uboLayoutBinding);
    }


    VkDescriptorSetLayoutCreateInfo layoutInfo = vkinit::descriptorSetLayoutCreateInfo(
        layoutBindings.data(),
        static_cast<uint32_t>(layoutBindings.size()));

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout));

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
        vkinit::pipelineLayoutCreateInfo(
            &descriptorSetLayout,
            1);

    VK_CHECK_RESULT(vkCreatePipelineLayout(device.getDevice(), &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout));
}


void VulkanShader::createDescriptorPool() 
{
    SPDLOG_TRACE("createDescriptorPool");

    auto swapchainImages =  swapchain.getSwapchianImageSize();
    std::vector<VkDescriptorPoolSize> poolSize{};

    for(auto& imageBinding : shaderBindings.imageBindings){
        auto samplerPoolSize = vkinit::descriptorPoolSize(
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            static_cast<uint32_t>(swapchainImages));
        poolSize.push_back(samplerPoolSize);
    }
    for(auto& uboBinding : shaderBindings.uboBindings){
        auto uboPoolSize = vkinit::descriptorPoolSize(
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                static_cast<uint32_t>(swapchainImages)); 
        poolSize.push_back(uboPoolSize);
    }

    VkDescriptorPoolCreateInfo poolInfo =
        vkinit::descriptorPoolCreateInfo(
            static_cast<uint32_t>(poolSize.size()),
            poolSize.data(),
            static_cast<uint32_t>(swapchainImages));

    VK_CHECK_RESULT(vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool));
 
}

void VulkanShader::createDescriptorSets() 
{
    SPDLOG_TRACE("createDescriptorSets");

    auto swapchainImages =  swapchain.getSwapchianImageSize();

    std::vector<VkDescriptorSetLayout> layouts(swapchainImages, descriptorSetLayout);
 
    auto allocInfo = vkinit::descriptorSetAllocateInfo(
        descriptorPool,
        layouts.data(),
        static_cast<uint32_t>(swapchainImages)
    );

    descriptorSets.resize(swapchainImages);
    VK_CHECK_RESULT(vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets.data()));

    // populate every descriptor:
    for (size_t i = 0; i < swapchainImages; i++) {
        
        std::vector<VkWriteDescriptorSet> descriptorWrites{};
        for(auto& imageBinding : shaderBindings.imageBindings){
            auto& imageview = imageBinding.second->getTextureImageView();
            auto& sampler = imageBinding.second->getTextureSampler();
            VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = imageview;
                imageInfo.sampler =  sampler;

            auto imageWrite = vkinit::writeDescriptorSet(
                descriptorSets[i],
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                imageBinding.first,
                &imageInfo
            );

            descriptorWrites.push_back(imageWrite); 
        }
        for(auto& uboBinding : shaderBindings.uboBindings){
            auto& uniformBuffers = uboBinding.second->getUniformBuffers();
            VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBuffers[i];
                bufferInfo.range = sizeof(UniformBufferObject);
            auto uboWrite = vkinit::writeDescriptorSet(
                descriptorSets[i],
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                uboBinding.first,
                &bufferInfo
            );

            descriptorWrites.push_back(uboWrite);
        }

        vkUpdateDescriptorSets(device.getDevice(), 
                static_cast<uint32_t>(descriptorWrites.size()), 
                descriptorWrites.data(), 
                0, nullptr);
    }
}
 

void VulkanShader::updateUbo(UniformBufferObject & mvp)
{
    auto& ubo = *(shaderBindings.uboBindings.at(globalUboBinding));

    ubo.model = mvp.model;
    ubo.view  = mvp.view;
    ubo.proj  = mvp.proj;
    ubo.proj[1][1] *= -1;
}

void VulkanShader::createPipeline() 
{ 
    
    // Fixed functions  
    // Vertex input
    // Interleaved vertex attributes 
	// Multiple bindings (for each attribute buffer) and multiple attribues
	const std::vector<VkVertexInputBindingDescription> vertexInputBindingsInterleaved = {
		{ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
	};
	const std::vector<VkVertexInputAttributeDescription> vertexInputAttributesInterleaved = {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
		{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) },
		{ 3, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex, texCoord) },
	};

    //connect the vertex input info to the one we get from Vertex
    const std::vector<VkVertexInputBindingDescription> descr{};
    const std::vector<VkVertexInputAttributeDescription> attr{};
    auto vertexInputState = vkinit::pipelineVertexInputStateCreateInfo(
        vertexInputBindingsInterleaved,
        vertexInputAttributesInterleaved
    );

    // Input assembly 
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        vkinit::pipelineInputAssemblyStateCreateInfo(
            topology, // default topogy is topology is VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
            0,
            VK_FALSE);

    // Rasterizer   
    VkPipelineRasterizationStateCreateInfo rasterizationState =
        vkinit::pipelineRasterizationStateCreateInfo(
            polygonMode, // default polygonmode is VK_POLYGON_MODE_FILL
            VK_CULL_MODE_NONE,
            VK_FRONT_FACE_COUNTER_CLOCKWISE,
            0);

    // Multisampling 
    auto samples_count = device.getMsaaSamples();
    VkPipelineMultisampleStateCreateInfo multisampleState =
    vkinit::pipelineMultisampleStateCreateInfo(
        samples_count,
        0);

    // Color blending attachements
    VkPipelineColorBlendAttachmentState blendAttachmentState =
    vkinit::pipelineColorBlendAttachmentState(
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | 
        VK_COLOR_COMPONENT_A_BIT,
        VK_FALSE);

    // Color blending state
    VkPipelineColorBlendStateCreateInfo colorBlendState =
    vkinit::pipelineColorBlendStateCreateInfo(
        1,
        &blendAttachmentState);

    // Depth and stencil state
    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        vkinit::pipelineDepthStencilStateCreateInfo(
            VK_TRUE,
            VK_TRUE,
            VK_COMPARE_OP_LESS_OR_EQUAL);

    // Vieewport 
    VkPipelineViewportStateCreateInfo viewportState =
    vkinit::pipelineViewportStateCreateInfo(1, 1, 0);

    //make viewport state dynamic.
    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
    vkinit::pipelineDynamicStateCreateInfo(
        dynamicStateEnables.data(),
        static_cast<uint32_t>(dynamicStateEnables.size()),
        0);

    // end 
    // Fixed functions

    //build the actual pipeline    
    auto renderPass = swapchain.getRenderpass();
    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
    vkinit::pipelineCreateInfo(
        pipelineLayout,
        renderPass,
        0);

    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline));
}
