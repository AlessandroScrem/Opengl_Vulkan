#include "VulkanEngine.hpp"

//std
#include <iostream>
#include <vector>

// lib

   

VulkanEngine::VulkanEngine()
{  
    SPDLOG_TRACE("constructor");

    createCommandBuffers();
    SPDLOG_TRACE("createCommandBuffers");

    createSyncObjects();
    SPDLOG_TRACE("createSyncObjects");
}

VulkanEngine::~VulkanEngine() 
{
    SPDLOG_TRACE("destructor");

    cleanupCommandBuffers();
    SPDLOG_TRACE("cleanupCommandBuffers");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device.getDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.getDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.getDevice(), inFlightFences[i], nullptr);
    }
    SPDLOG_TRACE("destroySyncObjects");
}

void VulkanEngine::run() 
{
    while(!window.shouldClose() ) {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(device.getDevice());   
}

// Necessita:
// swapchain.getSwapchain
// device getGraphicsQueue
// device.getPresentQueue()
void VulkanEngine::drawFrame() 
{   
    vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    
    // Acquiring an image from the swap chain
    //
    uint32_t imageIndex;
    VkResult  result = vkAcquireNextImageKHR(device.getDevice() , 
        swapchain.getSwapchain(), UINT64_MAX, 
        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, 
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device.getDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];
 
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //  GPU-GPU synchronization with semaphores
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Presentation
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain.getSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized()){
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}

// Necessita:
// swapchain.getSwapchianImageSize()
void VulkanEngine::createSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapchain.getSwapchianImageSize(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    
    }
}


// TODO
//  
// The disadvantage of this approach is that we need to stop all rendering before creating the new swap chain.
// 
// It is possible to create a new swap chain while drawing commands 
// on an image from the old swap chain are still in-flight. 
// You need to pass the previous swap chain to the oldSwapChain field 
// in the VkSwapchainCreateInfoKHR struct and destroy the old swap chain as soon as you’ve finished using it.

// Necessita:
// window.iconified()
// pipeline.cleanupPipeline();
// pipeline.createPipeline();
// swapchain.cleanupSwapChain();
// swapchain.createAllSwapchian();
void VulkanEngine::recreateSwapChain() 
{  
     while (window.waitforSize()) {
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(device.getDevice());

    cleanupCommandBuffers();
    pipeline.cleanupPipeline();
    swapchain.cleanupSwapChain();

    swapchain.createAllSwapchian();
    pipeline.createPipeline();
    createCommandBuffers();

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
// vertexbuffer.getVertexBuffer()
void VulkanEngine::createCommandBuffers() 
{
    commandBuffers.resize(swapchain.getFramebuffersSize());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device.getCommadPool();
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
                
                //TODO modify call to vertexbuffer and offset
                VkBuffer vertexBuffers[] = {vertexBuffer.getVertexBuffer()};
                VkBuffer indevBuffer = vertexBuffer.getIndexBuffer();
                size_t indexsize = vertexBuffer.getIndexSize();
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffers[i], indevBuffer, 0, VK_INDEX_TYPE_UINT16);

                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indexsize), 1, 0, 0, 0);
            vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

}


void VulkanEngine::cleanupCommandBuffers()
{
    vkFreeCommandBuffers(device.getDevice(), 
                        device.getCommadPool(), 
                        static_cast<uint32_t>(commandBuffers.size()), 
                        commandBuffers.data());    
}


