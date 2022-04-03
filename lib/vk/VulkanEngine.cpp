#include "VulkanEngine.hpp"
#include "vk_initializers.h"
//std
#include <vector>

VulkanEngine::VulkanEngine()
{  
    SPDLOG_TRACE("constructor");

    init_renderables();
    SPDLOG_TRACE("init_randerables");    

    init_commands();
    SPDLOG_TRACE("createCommandBuffers");

	init_sync_structures();  
    SPDLOG_TRACE("createSyncObjects");

}

VulkanEngine::~VulkanEngine() 
{
    SPDLOG_TRACE("destructor");

    _mainDeletionQueue.flush();
}

void VulkanEngine::run() 
{
    SPDLOG_TRACE("*******           START           ************");  

    while(!window.shouldClose() ) {
        glfwPollEvents();
        Engine::updateEvents();
        window.update();
        updateUbo();
        draw();
    }
    vkDeviceWaitIdle(device.getDevice()); 

    SPDLOG_TRACE("*******           END             ************");  
}

void VulkanEngine::init_renderables()
{
    _shaders.emplace_back(std::make_unique<VulkanShader>(device, Engine::phong_glslShader) );  
    auto  &sh = *_shaders.at(0);

    for(auto & mod : _models)
    {
        std::unique_ptr<VulkanVertexBuffer> vb = std::make_unique<VulkanVertexBuffer>(device, swapchain, ubo, vulkanimage, mod);
        std::unique_ptr<VulkanPipeline> pip = std::make_unique<VulkanPipeline>(device, swapchain, *vb, sh);
    
        _renderables.emplace_back(
            std::move(vb),
            std::move(pip), 
            ubo
        ); 
    }
}


/* void VulkanEngine::drawFrame() 
{   
    VK_CHECK(vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX) );
    
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
        VK_CHECK(vkWaitForFences(device.getDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX) );
    }

    // Mark the image as now being in use by this frame
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    ubo.bind(imageIndex);
 
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
    
    VK_CHECK(vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]) );

    VK_CHECK(vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) );

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
 */
 
void VulkanEngine::updateUbo()
{
    // rotate model to y up
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window.getWindowAspect(), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    ubo.viewPos = ourCamera.GetPosition();
} 


void VulkanEngine::recreateSwapChain() 
{  
     while (window.waitforSize()) {
        window.GetWindowExtents();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.getDevice());

// destroy
    for (auto  & element : _renderables)
    { 
        element.pipeline->cleanupPipeline ();
        element.vertexbuffer->cleanupDescriptorPool();
    }
    ubo.cleanupUniformBuffers();
    swapchain.cleanupSwapChain();

// create
    swapchain.createAllSwapchian();
    ubo.createUniformBuffers();
    for (auto & element : _renderables)
    { 
        element.vertexbuffer->createDescriptorPool();
        element.vertexbuffer->createDescriptorSets();
        element.pipeline->createPipeline();
    }
}



void VulkanEngine::init_sync_structures()
{
	//create syncronization structures
	//one fence to control when the gpu has finished rendering the frame,
	//and 2 semaphores to syncronize rendering with swapchain
	//we want the fence to start signalled so we can wait on it on the first frame
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

	VK_CHECK(vkCreateFence(device.getDevice(), &fenceCreateInfo, nullptr, &_renderFence));

	//enqueue the destruction of the fence
	_mainDeletionQueue.push_function([=]() {
		vkDestroyFence(device.getDevice(), _renderFence, nullptr);
		});
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	VK_CHECK(vkCreateSemaphore(device.getDevice(), &semaphoreCreateInfo, nullptr, &_presentSemaphore));
	VK_CHECK(vkCreateSemaphore(device.getDevice(), &semaphoreCreateInfo, nullptr, &_renderSemaphore));
	
	//enqueue the destruction of semaphores
	_mainDeletionQueue.push_function([=]() {
		vkDestroySemaphore(device.getDevice(), _presentSemaphore, nullptr);
		vkDestroySemaphore(device.getDevice(), _renderSemaphore, nullptr);
		});
}

void VulkanEngine::init_commands()
{
 	//create a command pool for commands submitted to the graphics queue.
    device.createCommandPool(&_commandPool);

	//allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(device.getDevice(), &cmdAllocInfo, &_mainCommandBuffer));

	_mainDeletionQueue.push_function([=]() {
		vkDestroyCommandPool(device.getDevice(), _commandPool, nullptr);
	});
}


void VulkanEngine::draw()
{
	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(device.getDevice(), 1, &_renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(device.getDevice(), 1, &_renderFence));

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(_mainCommandBuffer, 0));

	//request image from the swapchain
	uint32_t swapchainImageIndex;
	VkResult  result = vkAcquireNextImageKHR(device.getDevice(), swapchain.getSwapchain(), 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

	//naming it cmd for shorter writing
	VkCommandBuffer cmd = _mainCommandBuffer;

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	//make a clear-color.
	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	//clear depth at 1
	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.f;

	//start the main renderpass. 
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(swapchain.getRenderpass(), swapchain.getExtent(), swapchain.getFramebuffer(swapchainImageIndex));

	//connect clear values
	rpInfo.clearValueCount = 2;

	VkClearValue clearValues[] = { clearValue, depthClear };

	rpInfo.pClearValues = &clearValues[0];

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

        RenderObject & ro = _renderables.at(_model_index);

        VulkanPipeline &pipeline = *ro.pipeline;
        VulkanVertexBuffer &vertexbuffer = *ro.vertexbuffer;

	    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());

        ro.ubo.bind(0);

        VkBuffer vertexBuffers[] = {vertexbuffer.getVertexBuffer()};
        VkBuffer indexBuffer = vertexbuffer.getIndexBuffer();
        size_t indexsize = vertexbuffer.getIndexSize();
        VkDeviceSize offsets[] = {0};
        VkDescriptorSet descriptorSet = vertexbuffer.getDescriptorSet(0);
        VkPipelineLayout pipelineLayout = pipeline.getPipelineLayout();

        vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);       
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indexsize), 1, 0, 0, 0);

	//finalize the render pass
	vkCmdEndRenderPass(cmd);
	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));

	//prepare the submission to the queue. 
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	VkSubmitInfo submit = vkinit::submit_info(&cmd);
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &_presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &_renderSemaphore;

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit(device.getPresentQueue(), 1, &submit, _renderFence));

	//prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = vkinit::present_info();

    VkSwapchainKHR swapChains[] = {swapchain.getSwapchain()};
	presentInfo.pSwapchains = swapChains;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

    result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized()){
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

	//increase the number of frames drawn
	_frameNumber++;    
}

void VulkanEngine::draw_objects(VkCommandBuffer cmd,RenderObject* first, int count)
{
    
}

