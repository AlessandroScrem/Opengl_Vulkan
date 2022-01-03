#include "VulkanCommandBuffer.hpp"


VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice &device, VulkanSwapchain &swapchian, VulkanPipeline &pipeline) 
    : device{device}
    , swapchain{swapchian} 
    , pipeline{pipeline}
{
    std::cout << "VulkanCommandBuffer  constructor\n";

    createCommandPool();
    createCommandBuffers();
}

VulkanCommandBuffer::~VulkanCommandBuffer() 
{
    std::cout << "VulkanCommandBuffer  destructor\n";

    vkDestroyCommandPool(device.getDevice(), commandPool, nullptr);
}

void VulkanCommandBuffer::cleanupCommandBuffers() 
{
    vkFreeCommandBuffers(device.getDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());    
}

// Necessita:
// device.findPhysicalQueueFamilies
void VulkanCommandBuffer::createCommandPool() 
{
    QueueFamilyIndices queueFamilyIndices = device.findPhysicalQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0; // Optional  

    if (vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
    }
}

// Basic drawing commands

// Parameters, aside from the command buffer:
//
// vertexCount: Even though we don’t have a vertex buffer, we technically still have 3 vertices to draw.
// instanceCount: Used for instanced rendering, use 1 if you’re not doing that.
// firstVertex: Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
// firstInstance: Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.
// 
// Necessita:
// swapchain.getFramebuffersSize
// swapchain.getRenderpass
// swapchain.getFramebuffer
// swapchain.getExtent
// pipeline.getGraphicsPipeline
void VulkanCommandBuffer::createCommandBuffers() 
{
    commandBuffers.resize(swapchain.getFramebuffersSize());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain.getRenderpass();
        renderPassInfo.framebuffer = swapchain.getFramebuffer(i);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.getExtent(); 

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline() );
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

}