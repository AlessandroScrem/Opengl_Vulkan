#include "VulkanEngine.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanShader.hpp"
#include "vk_initializers.h"
//common lib
#include <Window.hpp>
#include "model.hpp"
//std
#include <vector>
#include <memory>

VulkanEngine::VulkanEngine(EngineType type) : Engine(type)
{ 
    SPDLOG_DEBUG("constructor");
    init();
}

VulkanEngine::~VulkanEngine() 
{
    SPDLOG_DEBUG("destructor");
    cleanup();
}

void VulkanEngine::init()
{
    device_ = std::make_unique<VulkanDevice>(*window_);
    swapchain_ = std::make_unique<VulkanSwapchain>(*device_, *window_); 
    Shader::addBuilder(std::make_unique<VulkanShaderBuilder>(*device_, *swapchain_));
    RenderObject::addBuilder(std::make_unique<VulkanObjectBuilder>(*device_));

    Engine::init_shaders(); 
    Engine::init_fixed();           
    Engine::init_renderables();

    init_commands();               
	init_sync_structures();

    if(ui_Overlay_){
        UIoverlay.windowPtr = window_->getWindowPtr();
        UIoverlay.device = device_.get();
        UIoverlay.swapchain = swapchain_.get();

        UIoverlay.init();
    }  
}

void VulkanEngine::cleanup() 
{
    vkDeviceWaitIdle(device_->getDevice()); 

    // cleanup_UiOverlay();
    if(ui_Overlay_){
        UIoverlay.cleanup();
    }

    _mainDeletionQueue.flush();

    // destroy Vulakan resources on Engine
    Engine::shaders_.clear();
    Engine::renderables_.clear();
    Engine::fixed_objects_.clear();
}

void VulkanEngine::recreateSwapChain() 
{  
    SPDLOG_DEBUG("recreateSwapChain");

    vkDeviceWaitIdle(device_->getDevice());
    // destroy 
    swapchain_->cleanupSwapChain();
    // create 
    swapchain_->createAllSwapchian();
}

void VulkanEngine::init_sync_structures()
{
    SPDLOG_TRACE("createSyncObjects");

    _presentSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    _renderSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFence.resize(MAX_FRAMES_IN_FLIGHT);

	//create syncronization structures
	//one fence to control when the gpu has finished rendering the frame,
	//and 2 semaphores to syncronize rendering with swapchain
	//we want the fence to start signalled so we can wait on it on the first frame
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VK_CHECK_RESULT(vkCreateSemaphore(device_->getDevice(), &semaphoreCreateInfo, nullptr, &_presentSemaphore[i]));
        VK_CHECK_RESULT(vkCreateSemaphore(device_->getDevice(), &semaphoreCreateInfo, nullptr, &_renderSemaphore[i]));
	    VK_CHECK_RESULT(vkCreateFence(device_->getDevice(), &fenceCreateInfo, nullptr, &_renderFence[i]));

        //enqueue the destruction of semaphores
        _mainDeletionQueue.push_function([=]() {
            vkDestroySemaphore(device_->getDevice(), _presentSemaphore[i], nullptr);
            vkDestroySemaphore(device_->getDevice(), _renderSemaphore[i], nullptr);
            });
        //enqueue the destruction of the fence
        _mainDeletionQueue.push_function([=]() {
            vkDestroyFence(device_->getDevice(), _renderFence[i], nullptr);
            });   
    }
}	

void VulkanEngine::init_commands()
{ 
    SPDLOG_TRACE("createCommandBuffers");
  
    _commandPool.resize(MAX_FRAMES_IN_FLIGHT);
    _mainCommandBuffer.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //create a command pool for commands submitted to the graphics queue.
        device_->createCommandPool(&_commandPool[i]);

        //allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool[i], 1);
	    VK_CHECK_RESULT(vkAllocateCommandBuffers(device_->getDevice(), &cmdAllocInfo, &_mainCommandBuffer[i]));

        _mainDeletionQueue.push_function([=]() {
            vkDestroyCommandPool(device_->getDevice(), _commandPool[i], nullptr);
        });
    }
}


void VulkanEngine::resizeFrame()
{
    recreateSwapChain(); 
}

void VulkanEngine::begin_frame()
{

	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK_RESULT(vkWaitForFences(device_->getDevice(), 1, &_renderFence[_currentFrame], true, 1000000000) );
	VK_CHECK_RESULT(vkResetFences(device_->getDevice(), 1, &_renderFence[_currentFrame]) );

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK_RESULT(vkResetCommandBuffer(_mainCommandBuffer[_currentFrame], /*VkCommandBufferResetFlagBits*/ 0));

	//request image from the swapchain
    VkResult  result = swapchain_->acquireNextImage(_presentSemaphore[_currentFrame], &swapchainImageIndex_);
	if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {
		recreateSwapChain();
	}
	else {
		VK_CHECK_RESULT(result);
	}

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK_RESULT(vkBeginCommandBuffer(_mainCommandBuffer[_currentFrame], &cmdBeginInfo)); 
}

void VulkanEngine::end_frame()
{
    //finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK_RESULT(vkEndCommandBuffer(_mainCommandBuffer[_currentFrame]));

	//prepare the submission to the queue. 
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	VkSubmitInfo submit = vkinit::submit_info(&_mainCommandBuffer[_currentFrame]);
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;
	submit.pWaitSemaphores = &_presentSemaphore[_currentFrame];
	submit.pSignalSemaphores = &_renderSemaphore[_currentFrame];

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK_RESULT(vkQueueSubmit(device_->getPresentQueue(), 1, &submit, _renderFence[_currentFrame]));

	//prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
    VkResult  result = swapchain_->queuePresent(device_->getPresentQueue(), swapchainImageIndex_, _renderSemaphore[_currentFrame]);
	if (!((result == VK_SUCCESS) || (result == VK_SUBOPTIMAL_KHR))) {
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			// Swap chain is no longer compatible with the surface and needs to be recreated
			recreateSwapChain();
			return;
		} else {
			VK_CHECK_RESULT(result);
		}
	}

	//next frame 0 -> MAX_FRAMES_IN_FLIGHT -1    
	_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;    
}

void VulkanEngine::begin_renderpass()
{
        //start the main renderpass. 
        //We will use the clear color from above, and the framebuffer of the index the swapchain gave us
        VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(
                                                swapchain_->getRenderpass(), 
                                                swapchain_->getExtent(), 
                                                swapchain_->getFramebuffer(swapchainImageIndex_)
                                                );
        // set the background color       
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{Engine::background.r, Engine::background.g, Engine::background.b, Engine::background.a}};
        clearValues[1].depthStencil.depth = {1.0f};
        //connect clear values
        rpInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        rpInfo.pClearValues = clearValues.data();


    //start the render pass
    vkCmdBeginRenderPass(_mainCommandBuffer[_currentFrame], &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanEngine::end_renderpass()
{
    //finalize the render pass
    vkCmdEndRenderPass(_mainCommandBuffer[_currentFrame]);
}

void VulkanEngine::draw()
{

    begin_frame();
    begin_renderpass();

            //initialize the viewport
            VkViewport viewport = vkinit::viewport(swapchain_->getExtent(), 0.0f, 1.0f);
            VkRect2D scissor = vkinit::rect2D(swapchain_->getExtent(), 0, 0);
            vkCmdSetViewport(_mainCommandBuffer[_currentFrame], 0, 1, &viewport);
            vkCmdSetScissor(_mainCommandBuffer[_currentFrame], 0, 1, &scissor);   

            draw_objects(_mainCommandBuffer[_currentFrame]);
            draw_fixed(_mainCommandBuffer[_currentFrame]);

            if(ui_Overlay_){
                UIoverlay.newFrame();
                        Engine::draw_UiOverlay();
                UIoverlay.draw(_mainCommandBuffer[_currentFrame]);
            }

    end_renderpass();
    end_frame();

}

void VulkanEngine::draw_objects(VkCommandBuffer cmd)
{
    for(  auto & ro : renderables_){

        VulkanShader &shader                = static_cast<VulkanShader&>(Engine::getShader(ro->shader));
        VulkanVertexBuffer &vertexbuffer    = static_cast<VulkanVertexBuffer&>(*ro);

        UniformBufferObject mvp = Engine::getMVP();        
        mvp.model = ro->objNode.getfinal();
        shader.updateUbo(mvp);
        
        shader.bind(cmd);
        vertexbuffer.draw(cmd);
     }   
}

void VulkanEngine::draw_fixed(VkCommandBuffer cmd)
{
        
    RenderObject & ro                   = *fixed_objects_.at("axis");
    VulkanShader & shader               = static_cast<VulkanShader&>(Engine::getShader(ro.shader));
    VulkanVertexBuffer &vertexbuffer    = static_cast<VulkanVertexBuffer&>(ro);
    
    auto[x, y] = window_->extents();
    //set new world origin to bottom left + offset
    float offset = 50; 
    float left   = -offset;
    float right  = x-offset;
    float bottom = y-offset;
    float top    = -offset;

    UniformBufferObject mvp{};
    mvp.view = ourCamera.GetViewMatrix();
    mvp.proj = glm::orthoLH_ZO(left, right, bottom, top, -100.0f, 100.0f);
    mvp.proj[1][1] *= -1;
    shader.updateUbo(mvp);

    shader.bind(cmd);
    vertexbuffer.draw(cmd);       
}
