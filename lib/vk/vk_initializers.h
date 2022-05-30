#pragma once
#include "vktypes.h"

namespace vkinit {

    // *********** SYNC STRUCTURE  **************//
    // 
	inline VkFenceCreateInfo fence_create_info(
        VkFenceCreateFlags flags = 0)
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = flags;

		return info;		
	}
	
	inline VkSemaphoreCreateInfo semaphore_create_info(
        VkSemaphoreCreateFlags flags = 0)
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = flags;
		return info;		
	}

    /** @brief Initialize an image memory barrier with no image transfer ownership */
    inline VkImageMemoryBarrier imageMemoryBarrier()
    {
        VkImageMemoryBarrier imageMemoryBarrier {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        return imageMemoryBarrier;
    }

    // *********** IMAGE  **************//
    // 	
	inline VkImageCreateInfo image_create_info(
        VkFormat format, 
        VkImageUsageFlags usageFlags, 
        VkExtent3D extent,
        VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT,  
        uint32_t mipLevels = 1)
	{
		VkImageCreateInfo info = { };
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.pNext = nullptr;

		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = format;
		info.extent = extent;
		info.mipLevels = mipLevels;
		info.arrayLayers = 1;
		info.samples = numSamples;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = usageFlags;

		return info;
	}
	
	inline VkImageViewCreateInfo imageview_create_info(
        VkFormat format, 
        VkImage image, 
        VkImageAspectFlags aspectFlags, 
        uint32_t mipLevels = 1)
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.pNext = nullptr;

		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.image = image;
		info.format = format;
		info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = mipLevels;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;
		info.subresourceRange.aspectMask = aspectFlags;

		return info;
	}

    inline VkSamplerCreateInfo samplerCreateInfo()
    {
        VkSamplerCreateInfo samplerCreateInfo {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        return samplerCreateInfo;
    }

    // *********** COMMAND  **************//
    // 	
    inline VkCommandPoolCreateInfo command_pool_create_info(
        uint32_t queueFamilyIndex, 
        VkCommandPoolCreateFlags flags = 0)
	{
		VkCommandPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;

		info.queueFamilyIndex = queueFamilyIndex;
		info.flags = flags;
		return info;
	}

	inline VkCommandBufferAllocateInfo command_buffer_allocate_info(
        VkCommandPool pool, uint32_t count = 1, 
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;

		info.commandPool = pool;
		info.commandBufferCount = count;
		info.level = level;
		return info;
	}

	inline VkCommandBufferBeginInfo command_buffer_begin_info(
        VkCommandBufferUsageFlags flags = 0)
	{
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;

		info.pInheritanceInfo = nullptr;
		info.flags = flags;
		return info;
	}

    // *********** BUFFER  **************//
    // 
    inline	VkBufferCreateInfo vertex_input_state_create_info(
        VkDeviceSize size, 
        VkBufferUsageFlags usage)
	{
		VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.size = size;
		info.usage = usage;

		return info;
	}
	
    // *********** FRAMEBUFFER  **************//
    // 
	inline VkFramebufferCreateInfo framebuffer_create_info(
        VkRenderPass renderPass, 
        VkExtent2D extent)
	{
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = nullptr;

		info.renderPass = renderPass;
		info.attachmentCount = 1;
		info.width = extent.width;
		info.height = extent.height;
		info.layers = 1;

		return info;		
	}

    inline VkViewport viewport(
        float width,
        float height,
        float minDepth,
        float maxDepth)
    {
        VkViewport viewport {};
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        return viewport;
    }

    inline VkViewport viewport(
        VkExtent2D extent,
        float minDepth,
        float maxDepth)
    {
        VkViewport viewport {};
        viewport.width = extent.width;
        viewport.height = extent.height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        return viewport;
    }

    inline VkRect2D rect2D(
        VkExtent2D extent,
        int32_t offsetX,
        int32_t offsetY)
    {
        VkRect2D rect2D {};
        rect2D.extent.width = extent.width;
        rect2D.extent.height = extent.height;
        rect2D.offset.x = offsetX;
        rect2D.offset.y = offsetY;
        return rect2D;
    }

    inline VkRect2D rect2D(
        int32_t width,
        int32_t height,
        int32_t offsetX,
        int32_t offsetY)
    {
        VkRect2D rect2D {};
        rect2D.extent.width = width;
        rect2D.extent.height = height;
        rect2D.offset.x = offsetX;
        rect2D.offset.y = offsetY;
        return rect2D;
    }

	inline VkPresentInfoKHR present_info()
	{
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext = nullptr;

		info.swapchainCount = 1;
		info.pSwapchains = nullptr;
		info.pWaitSemaphores = nullptr;
		info.waitSemaphoreCount = 1;
		info.pImageIndices = nullptr;

		return info;		
	}

    inline VkSubmitInfo submit_info(
        VkCommandBuffer* cmd)
	{
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;

		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = nullptr;
		info.commandBufferCount = 1;
		info.pCommandBuffers = cmd;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = nullptr;

		return info;		
	}
	
	inline VkRenderPassBeginInfo renderpass_begin_info(
        VkRenderPass renderPass, 
		VkExtent2D windowExtent, 
		VkFramebuffer framebuffer)
	{

		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.pNext = nullptr;
		info.renderPass = renderPass;
		info.renderArea.offset.x = 0;
		info.renderArea.offset.y = 0;
		info.renderArea.extent = windowExtent;
		info.clearValueCount = 1;
		info.pClearValues = nullptr;
		info.framebuffer = framebuffer;

		return info;		
	}

    // *********** DESCRIPTORS  **************//
    // 
    inline VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
        VkDescriptorPool descriptorPool,
        const VkDescriptorSetLayout* pSetLayouts,
        uint32_t descriptorSetCount)
    {
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;
        descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
        return descriptorSetAllocateInfo;
    }

    inline VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(
        VkDescriptorType type,
        VkShaderStageFlags stageFlags,
        uint32_t binding,
        uint32_t descriptorCount = 1)
    {
        VkDescriptorSetLayoutBinding setLayoutBinding {};
        setLayoutBinding.descriptorType = type;
        setLayoutBinding.stageFlags = stageFlags;
        setLayoutBinding.binding = binding;
        setLayoutBinding.descriptorCount = descriptorCount;
        return setLayoutBinding;
    }

    inline VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
        const VkDescriptorSetLayoutBinding* pBindings,
        uint32_t bindingCount)
    {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.pBindings = pBindings;
        descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
        return descriptorSetLayoutCreateInfo;
    }

    inline VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(
        uint32_t poolSizeCount,
        VkDescriptorPoolSize* pPoolSizes,
        uint32_t maxSets)
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo {};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = poolSizeCount;
        descriptorPoolInfo.pPoolSizes = pPoolSizes;
        descriptorPoolInfo.maxSets = maxSets;
        return descriptorPoolInfo;
    }

    inline VkDescriptorPoolSize descriptorPoolSize(
        VkDescriptorType type,
        uint32_t descriptorCount)
    {
        VkDescriptorPoolSize descriptorPoolSize {};
        descriptorPoolSize.type = type;
        descriptorPoolSize.descriptorCount = descriptorCount;
        return descriptorPoolSize;
    }

    inline VkWriteDescriptorSet writeDescriptorSet(
        VkDescriptorSet dstSet,
        VkDescriptorType type,
        uint32_t binding,
        VkDescriptorBufferInfo* bufferInfo,
        uint32_t descriptorCount = 1)
    {
        VkWriteDescriptorSet writeDescriptorSet {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = dstSet;
        writeDescriptorSet.descriptorType = type;
        writeDescriptorSet.dstBinding = binding;
        writeDescriptorSet.pBufferInfo = bufferInfo;
        writeDescriptorSet.descriptorCount = descriptorCount;
        return writeDescriptorSet;
    }

    inline VkWriteDescriptorSet writeDescriptorSet(
        VkDescriptorSet dstSet,
        VkDescriptorType type,
        uint32_t binding,
        VkDescriptorImageInfo *imageInfo,
        uint32_t descriptorCount = 1)
    {
        VkWriteDescriptorSet writeDescriptorSet {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = dstSet;
        writeDescriptorSet.descriptorType = type;
        writeDescriptorSet.dstBinding = binding;
        writeDescriptorSet.pImageInfo = imageInfo;
        writeDescriptorSet.descriptorCount = descriptorCount;
        return writeDescriptorSet;
    }

    // *********** PIPELINE  **************//
    // 
    inline VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo()
    {
        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {};
        pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        return pipelineVertexInputStateCreateInfo;
    }

    inline VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
        const std::vector<VkVertexInputBindingDescription> &vertexBindingDescriptions,
        const std::vector<VkVertexInputAttributeDescription> &vertexAttributeDescriptions
    )
    {
        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
        pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
        pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
        pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
        return pipelineVertexInputStateCreateInfo;
    }

    inline VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
        VkPrimitiveTopology topology,
        VkPipelineInputAssemblyStateCreateFlags flags,
        VkBool32 primitiveRestartEnable)
    {
        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {};
        pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineInputAssemblyStateCreateInfo.topology = topology;
        pipelineInputAssemblyStateCreateInfo.flags = flags;
        pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
        return pipelineInputAssemblyStateCreateInfo;
    }

    inline VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
        VkPolygonMode polygonMode,
        VkCullModeFlags cullMode,
        VkFrontFace frontFace,
        VkPipelineRasterizationStateCreateFlags flags = 0)
    {
        VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
        pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
        pipelineRasterizationStateCreateInfo.cullMode = cullMode;
        pipelineRasterizationStateCreateInfo.frontFace = frontFace;
        pipelineRasterizationStateCreateInfo.flags = flags;
        pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
        return pipelineRasterizationStateCreateInfo;
    }

    inline VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
        VkColorComponentFlags colorWriteMask,
        VkBool32 blendEnable)
    {
        VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState {};
        pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
        pipelineColorBlendAttachmentState.blendEnable = blendEnable;
        return pipelineColorBlendAttachmentState;
    }

    inline VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
        uint32_t attachmentCount,
        const VkPipelineColorBlendAttachmentState * pAttachments)
    {
        VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo {};
        pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
        pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
        return pipelineColorBlendStateCreateInfo;
    }

    inline VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
        VkBool32 depthTestEnable,
        VkBool32 depthWriteEnable,
        VkCompareOp depthCompareOp)
    {
        VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo {};
        pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
        pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
        pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
        pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
        return pipelineDepthStencilStateCreateInfo;
    }

    inline VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(
        uint32_t viewportCount,
        uint32_t scissorCount,
        VkPipelineViewportStateCreateFlags flags = 0)
    {
        VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
        pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipelineViewportStateCreateInfo.viewportCount = viewportCount;
        pipelineViewportStateCreateInfo.scissorCount = scissorCount;
        pipelineViewportStateCreateInfo.flags = flags;
        return pipelineViewportStateCreateInfo;
    }

    inline VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
        VkSampleCountFlagBits rasterizationSamples,
        VkPipelineMultisampleStateCreateFlags flags = 0)
    {
        VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo {};
        pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
        pipelineMultisampleStateCreateInfo.flags = flags;
        return pipelineMultisampleStateCreateInfo;
    }

    inline VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
        const VkDynamicState * pDynamicStates,
        uint32_t dynamicStateCount,
        VkPipelineDynamicStateCreateFlags flags = 0)
    {
        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo {};
        pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
        pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
        pipelineDynamicStateCreateInfo.flags = flags;
        return pipelineDynamicStateCreateInfo;
    }

    inline VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
        const std::vector<VkDynamicState>& pDynamicStates,
        VkPipelineDynamicStateCreateFlags flags = 0)
    {
        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
        pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
        pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
        pipelineDynamicStateCreateInfo.flags = flags;
        return pipelineDynamicStateCreateInfo;
    }

    inline VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo(uint32_t patchControlPoints)
    {
        VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo {};
        pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
        return pipelineTessellationStateCreateInfo;
    }

    inline VkGraphicsPipelineCreateInfo pipelineCreateInfo(
        VkPipelineLayout layout,
        VkRenderPass renderPass,
        VkPipelineCreateFlags flags = 0)
    {
        VkGraphicsPipelineCreateInfo pipelineCreateInfo {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.layout = layout;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.flags = flags;
        pipelineCreateInfo.basePipelineIndex = -1;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        return pipelineCreateInfo;
    }

    inline VkGraphicsPipelineCreateInfo pipelineCreateInfo()
    {
        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.basePipelineIndex = -1;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        return pipelineCreateInfo;
    }

    inline VkComputePipelineCreateInfo computePipelineCreateInfo(
        VkPipelineLayout layout, 
        VkPipelineCreateFlags flags = 0)
    {
        VkComputePipelineCreateInfo computePipelineCreateInfo {};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.layout = layout;
        computePipelineCreateInfo.flags = flags;
        return computePipelineCreateInfo;
    }

    inline VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        const VkDescriptorSetLayout* pSetLayouts,
        uint32_t setLayoutCount = 1)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
        pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
        return pipelineLayoutCreateInfo;
    }

    inline VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        uint32_t setLayoutCount = 1)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
        return pipelineLayoutCreateInfo;
    }

    inline VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(
        VkShaderStageFlagBits stage, 
        VkShaderModule shaderModule)
	{
		VkPipelineShaderStageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.pNext = nullptr;

		info.stage = stage;
		info.module = shaderModule;
		info.pName = "main";
		return info;
	}
}