#include "VulkanEngine.hpp"
//std
#include <vector>


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
    SPDLOG_TRACE("**********************************************");  
    SPDLOG_TRACE("*******           START           ************");  
    SPDLOG_TRACE("**********************************************");  

    while(!window.shouldClose() ) {
        glfwPollEvents();
        Engine::updateEvents();
        window.update();
        drawFrame();
    }
    vkDeviceWaitIdle(device.getDevice()); 

    SPDLOG_TRACE("**********************************************");  
    SPDLOG_TRACE("*******           END             ************");  
    SPDLOG_TRACE("**********************************************");  
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

    updateUbo(imageIndex);
 
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

void VulkanEngine::updateUbo(uint32_t currentImage)
{
    // rotate model to y up
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window.getWindowAspect(), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    
    // update uniform buffer data
    ubo.bind(currentImage);

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
        window.update();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.getDevice());

// destroy
    cleanupCommandBuffers();
    pipeline.cleanupPipeline();
    //vertexbuffer.cleanupUniformBuffers();
    ubo.cleanupUniformBuffers();
    vertexbuffer.cleanupDescriptorPool();
    swapchain.cleanupSwapChain();

// create
    swapchain.createAllSwapchian();
    //vertexbuffer.createUniformBuffers();
    ubo.createUniformBuffers();
    vertexbuffer.createDescriptorPool();
    vertexbuffer.createDescriptorSets();
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
// vertexbuffer.getIndexBuffer()
// vertexbuffer.getDescriptorSet
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

            // set the background color
            float r = Engine::background.red;
            float g = Engine::background.green;
            float b = Engine::background.blue;
            float a = Engine::background.alpha;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {{r, g, b, a}};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();


            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline() );
                
                //TODO modify call to vertexbuffer and offset
                VkBuffer vertexBuffers[] = {vertexbuffer.getVertexBuffer()};
                VkBuffer indexBuffer = vertexbuffer.getIndexBuffer();
                size_t indexsize = vertexbuffer.getIndexSize();
                VkDeviceSize offsets[] = {0};
                VkDescriptorSet descriptorSet = vertexbuffer.getDescriptorSet(i);
                VkPipelineLayout pipelineLayout = pipeline.getPipelineLayout();

                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                
                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

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


