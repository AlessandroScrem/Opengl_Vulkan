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

class PipelineBuilder2 {
public:

	std::vector<VkPipelineShaderStageCreateInfo> *_shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo _depthStencil;

	VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
};

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
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.binding = imageBinding.first;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = nullptr;

        layoutBindings.push_back(samplerLayoutBinding);
    }
    for(auto& uboBinding : shaderBindings.uboBindings){
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.binding = uboBinding.first;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        layoutBindings.push_back(uboLayoutBinding);
    }


    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}


void VulkanShader::createDescriptorPool() 
{
    SPDLOG_TRACE("createDescriptorPool");

    auto swapchainImages =  swapchain.getSwapchianImageSize();

    std::vector<VkDescriptorPoolSize> poolSize{};

    for(auto& imageBinding : shaderBindings.imageBindings){
        VkDescriptorPoolSize samplerPoolSize{};
        samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerPoolSize.descriptorCount = static_cast<uint32_t>(swapchainImages);

        poolSize.push_back(samplerPoolSize);
    }
    for(auto& uboBinding : shaderBindings.uboBindings){
        VkDescriptorPoolSize uboPoolSize{};
        uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboPoolSize.descriptorCount = static_cast<uint32_t>(swapchainImages); 

        poolSize.push_back(uboPoolSize);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
    poolInfo.pPoolSizes = poolSize.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapchainImages);

    if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanShader::createDescriptorSets() 
{
    SPDLOG_TRACE("createDescriptorSets");

    auto swapchainImages =  swapchain.getSwapchianImageSize();

    std::vector<VkDescriptorSetLayout> layouts(swapchainImages, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainImages);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(swapchainImages);
    if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

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

            VkWriteDescriptorSet imageWrite{};
            imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            imageWrite.dstSet = descriptorSets[i];
            imageWrite.dstBinding = imageBinding.first;
            imageWrite.dstArrayElement = 0;
            imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            imageWrite.descriptorCount = 1;
            imageWrite.pImageInfo = &imageInfo;

            descriptorWrites.push_back(imageWrite); 
        }
        for(auto& uboBinding : shaderBindings.uboBindings){
            auto& uniformBuffers = uboBinding.second->getUniformBuffers();
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet uboWrite{};
            uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uboWrite.dstSet = descriptorSets[i];
            uboWrite.dstBinding = uboBinding.first;
            uboWrite.dstArrayElement = 0;
            uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboWrite.descriptorCount = 1;
            uboWrite.pBufferInfo = &bufferInfo;

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

    //build the pipeline layout that controls the inputs/outputs of the shader
	//we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::pipeline_layout_create_info();
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    VK_CHECK(vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) );  

    //build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
	PipelineBuilder2 pipelineBuilder;

    //use the default shaders
    pipelineBuilder._shaderStages = &shaderStages;
    
    //use the default layout we created
    pipelineBuilder._pipelineLayout = pipelineLayout;
  
    // Fixed functions
    // Vertex input
    pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

    //connect the pipeline builder vertex input info to the one we get from Vertex
    auto bindingDescription = getBindingDescription();
    auto attributeDescriptions = getAttributeDescriptions();

    pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
    pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
    pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
  
    // Input assembly
    // default topogy is topology is VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(topology);

    // Rasterizer
    // default polygonmode is VK_POLYGON_MODE_FILL
    pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(polygonMode, Opengl_compatible_viewport);

    // Multisampling 
    pipelineBuilder._multisampling = vkinit::multisampling_state_create_info(device.getMsaaSamples());
  
    // Depth and stencil state
    pipelineBuilder._depthStencil = vkinit::depth_stencil_create_info(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS);

    // Color blending
    pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

    // end 
    // Fixed functions
    graphicsPipeline = pipelineBuilder.build_pipeline(device.getDevice(), swapchain.getRenderpass()); 
}

VkPipeline PipelineBuilder2::build_pipeline(VkDevice device, VkRenderPass pass) {

	//make viewport state dynamic.
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;

	viewportState.viewportCount = 1;
	viewportState.pViewports = nullptr;
	viewportState.scissorCount = 1;
	viewportState.pScissors = nullptr;
    
    // Vieewport + Scissor 
    VkDynamicState dynamicStateEnables[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR}; 
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 2;

	//setup dummy color blending. We arent using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &_colorBlendAttachment;

	//build the actual pipeline
	//we now use all of the info structs we have been writing into into this one to create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
    pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.stageCount = static_cast<uint32_t>(_shaderStages->size());
	pipelineInfo.pStages = _shaderStages->data();
	pipelineInfo.pVertexInputState = &_vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &_inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &_rasterizer;
	pipelineInfo.pMultisampleState = &_multisampling;
    pipelineInfo.pDepthStencilState = &_depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = _pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	//it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(
		device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
		spdlog::error("failed to create pipeline");
		return VK_NULL_HANDLE; // failed to create graphics pipeline
	}
	else
	{
		return newPipeline;
	}
}


