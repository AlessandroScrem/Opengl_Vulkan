#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanUbo.hpp"
#include "VulkanImage.hpp"
#include "VulkanShader.hpp"
#include "vk_initializers.h"
// lib
// std
#include <string>

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
    auto image =  std::make_unique<VulkanImage>(device, imagepath);

    this->shader->shaderBindings.imageBindings.emplace(id, std::move(image));
    return *this;
}

ShaderBuilder& VulkanShaderBuilder::setPolygonMode(GLSL::PolygonMode mode) {
    switch (mode)
    {
    case GLSL::TRIANGLES:
        this->shader->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        // this->shader->polygonMode = VK_POLYGON_MODE_FILL;
        break;
    case GLSL::LINES:
        this->shader->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        // this->shader->polygonMode = VK_POLYGON_MODE_LINE;
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

    createPipeline(GLSL::TRIANGLES); 
    createPipeline(GLSL::LINES); 

    prepared = true;             
}

 void VulkanShader::bind(VkCommandBuffer cmd, GLSL::PolygonMode mode)
 {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline[mode]);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
 }   

void VulkanShader::cleanupDescriptorPool()
{
    SPDLOG_TRACE("cleanupDescriptorPool");
    vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
}

void VulkanShader::cleanupPipeline()
{
    SPDLOG_TRACE("vkDestroyPipeline");
    for( auto & pipeline : graphicsPipeline){
        vkDestroyPipeline(device.getDevice(), pipeline, nullptr);
    }
    SPDLOG_TRACE("vkDestroyPipelineLayout");
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);    
}


VkShaderModule VulkanShader::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    VK_CHECK_RESULT(vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, &shaderModule));
    
    return shaderModule;
}

void VulkanShader::buildShaders(){ 

    std::vector<char> vertshader_spv{};
    std::vector<char> fragshader_spv{};
    
    vertshader_spv = GLSL::readFile(GLSL::getPath(shaderType) + ".vert.spv");
    fragshader_spv = GLSL::readFile(GLSL::getPath(shaderType) + ".frag.spv");

    vertModule = createShaderModule(vertshader_spv);
    fragModule = createShaderModule(fragshader_spv);
  
    shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertModule));
        
    shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragModule));

}

void VulkanShader::createGlobalUbo() {
    auto ubo = std::make_unique<VulkanUbo>(device); 
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

    auto allocInfo = vkinit::descriptorSetAllocateInfo(
        descriptorPool,
        &descriptorSetLayout,
        1);

    VK_CHECK_RESULT(vkAllocateDescriptorSets(device.getDevice(), &allocInfo, &descriptorSet));
    
    std::vector<VkWriteDescriptorSet> writeDescriptorSets{};
    for(auto& imageBinding : shaderBindings.imageBindings){
        auto& imageview = imageBinding.second->getTextureImageView();
        auto& sampler = imageBinding.second->getTextureSampler();
        VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = imageview;
            imageInfo.sampler =  sampler;

        auto imageWrite = vkinit::writeDescriptorSet(
            descriptorSet,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            imageBinding.first,
            &imageInfo);

        writeDescriptorSets.push_back(imageWrite); 
    }
    for(auto& uboBinding : shaderBindings.uboBindings){
        auto& uniformBuffer = uboBinding.second->getUniformBuffer();
        VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffer;
            bufferInfo.range = sizeof(UniformBufferObject);
        auto uboWrite = vkinit::writeDescriptorSet(
            descriptorSet,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            uboBinding.first,
            &bufferInfo);

        writeDescriptorSets.push_back(uboWrite);
    }

    vkUpdateDescriptorSets(device.getDevice(), 
            static_cast<uint32_t>(writeDescriptorSets.size()), 
            writeDescriptorSets.data(), 
            0, nullptr);
}

void VulkanShader::updateUbo(UniformBufferObject & mvp)
{
    auto& ubo = *(shaderBindings.uboBindings.at(globalUboBinding));

    ubo.model = mvp.model;
    ubo.view  = mvp.view;
    ubo.proj  = mvp.proj;
    ubo.viewPos = mvp.viewPos;
    ubo.drawLines = mvp.drawLines;
    
    ubo.proj[1][1] *= -1;

    ubo.map();
}

void VulkanShader::createPipeline(GLSL::PolygonMode mode) 
{ 
    struct DrawMode{
        VkPrimitiveTopology topology;
        VkPolygonMode polygonMode;

    }drawMode;

    switch (mode)
    {
    case GLSL::TRIANGLES:
        drawMode.topology = VulkanShader::topology;
        drawMode.polygonMode = VK_POLYGON_MODE_FILL;
        break;
    
    case GLSL::LINES:
        drawMode.topology = VulkanShader::topology;
        drawMode.polygonMode = VK_POLYGON_MODE_LINE;
        break;
    }


    // Fixed functions  
    // Vertex input
    // Interleaved vertex attributes 
	// Multiple bindings (for each attribute buffer) and multiple attribues
	const std::vector<VkVertexInputBindingDescription> vertexInputBindingsInterleaved = {
		{ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
	};
	const std::vector<VkVertexInputAttributeDescription> vertexInputAttributesInterleaved = {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, pos)) },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, color)) },
		{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, normal)) },
		{ 3, 0, VK_FORMAT_R32G32_SFLOAT,    static_cast<uint32_t>(offsetof(Vertex, texCoord)) },
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
            drawMode.topology, // default topogy is topology is VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
            VK_FALSE);

    // Rasterizer   
    VkPipelineRasterizationStateCreateInfo rasterizationState =
        vkinit::pipelineRasterizationStateCreateInfo(
            drawMode.polygonMode, // default polygonmode is VK_POLYGON_MODE_FILL
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
        VK_DYNAMIC_STATE_SCISSOR,
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

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline[mode]));
}
