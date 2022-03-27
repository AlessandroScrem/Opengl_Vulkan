#include "VulkanPipeline.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanShader.hpp"
#include "vk_initializers.h"

VulkanPipeline::VulkanPipeline(VulkanDevice &device, VulkanSwapchain &swapchain, 
                                VulkanVertexBuffer &vertexbuffer, VulkanShader &vulkanshader) 
    : device{device}
    , swapchain{swapchain}
    , vertexbuffer{vertexbuffer}
    , vulkanshader{vulkanshader}
{
    SPDLOG_TRACE("constructor");
    createPipeline();
}

VulkanPipeline::~VulkanPipeline()
{
    SPDLOG_TRACE("destructor");

    cleanupPipeline();
}

void VulkanPipeline::cleanupPipeline()
{
    SPDLOG_TRACE("vkDestroyPipeline");
    vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
    SPDLOG_TRACE("vkDestroyPipelineLayout");
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);    
}

// Necessita
// swapchain.getExtent()
// swapchain.getRenderpass()
// vulkanshader.get()
void VulkanPipeline::createPipeline() 
{ 

    //build the pipeline layout that controls the inputs/outputs of the shader
	//we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::pipeline_layout_create_info();
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &vertexbuffer.getDescriptorSetLayout();

    VK_CHECK(vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) );  

    //build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
	PipelineBuilder pipelineBuilder;

    //use the default shaders
    pipelineBuilder._shaderStages = vulkanshader.get();
    
    //use the default layout we created
    pipelineBuilder._pipelineLayout = pipelineLayout;
  
    // Fixed functions
    // Vertex input
    pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

    //connect the pipeline builder vertex input info to the one we get from Vertex
    pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexbuffer.getAttributeDescriptions().data();
    pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( vertexbuffer.getAttributeDescriptions().size() );
    pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexbuffer.getBindingDescription().data();
    pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>( vertexbuffer.getBindingDescription().size() );
  
    // Input assembly
    pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    //Viewports and scissors 
    VkExtent2D swapChainExtent = swapchain.getExtent();
    float height = (float) swapChainExtent.height;
    float width = (float) swapChainExtent.width;
    float offsetx = 0.f;
    float offsety = 0.f;

    // Opengl compatible Viewport (SashaWillems)
    if(Opengl_compatible_viewport)
    {
        offsety =  height;
        height  = -height;
    }
    
    pipelineBuilder._viewport.y = offsety;
    pipelineBuilder._viewport.x = offsetx;
    pipelineBuilder._viewport.height = height;
    pipelineBuilder._viewport.width  = width;
    pipelineBuilder._viewport.minDepth = 0.0f;
    pipelineBuilder._viewport.maxDepth = 1.0f;

    pipelineBuilder._scissor.offset = {0, 0};
    pipelineBuilder._scissor.extent = swapChainExtent;

    // Rasterizer
    pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL, Opengl_compatible_viewport);

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

VkPipeline PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass) {

	//make viewport state from our stored viewport and scissor.
		//at the moment we wont support multiple viewports or scissors
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;

	viewportState.viewportCount = 1;
	viewportState.pViewports = &_viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &_scissor;

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

