#include "VulkanEngine.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanShader.hpp"
#include "vk_initializers.h"
//common lib
#include <Window.hpp>
#include "model.hpp"
//lib
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
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

    vkDeviceWaitIdle(device_->getDevice()); 

    cleanup_UiOverlay();

    _mainDeletionQueue.flush();

    // destroy Vulakan resources on Engine
    Engine::shaders_.clear();
    Engine::renderables_.clear();
    Engine::fixed_objects_.clear();
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
    init_UiOverlay();            
	init_sync_structures();  
}



void VulkanEngine::cleanup_UiOverlay()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    // TODO  maybe move to Gui class
    if(_gui_DescriptorPool) {
        vkDestroyDescriptorPool(device_->getDevice(), _gui_DescriptorPool, nullptr);
    }     
}

void VulkanEngine::init_UiOverlay()
{
    SPDLOG_TRACE("UiOverlay");

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window_->getWindowPtr(), true);

    VkDevice                 g_Device           = device_->getDevice();
    VkPhysicalDevice         g_PhysicalDevice   = device_->getPhysicalDevice();
    VkInstance               g_Instance         = device_->getInstance();
    uint32_t                 g_QueueFamily      = device_->getQueueFamiliesIndices().graphicsFamily.value(); //only grahics family;
    VkQueue                  g_Queue            = device_->getGraphicsQueue();                                 //only grahics Queue
    int                      g_MinImageCount    = device_->getSwapChainSupport().capabilities.minImageCount;
    int                      g_ImageCount       = g_MinImageCount + 1;
    VkRenderPass             g_RenderPass       = swapchain_->getRenderpass();
    VkSampleCountFlagBits    g_MSAASamples      = device_->getMsaaSamples();
    auto                     g_CheckVkResultFn  = [](VkResult x){ VK_CHECK_RESULT(x);};            
    VkAllocationCallbacks*   g_Allocator        = NULL;
    VkPipelineCache          g_PipelineCache    = VK_NULL_HANDLE;

    // Create Descriptor Pool
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        VK_CHECK_RESULT(vkCreateDescriptorPool(g_Device, &pool_info, NULL, &_gui_DescriptorPool));
    }

    // init ImGui_ImplVulkan
    {
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = g_Instance;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily =  g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = _gui_DescriptorPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = g_ImageCount;
        init_info.MSAASamples = g_MSAASamples;
        init_info.Allocator = g_Allocator;
        init_info.CheckVkResultFn = g_CheckVkResultFn;

        ImGui_ImplVulkan_Init(&init_info, g_RenderPass);
    }    

    // Upload Fonts
    {
        auto cmd = device_->beginSingleTimeCommands();
        
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        
        device_->endSingleTimeCommands(cmd);
        
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
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


void VulkanEngine::begin_frame()
{
    if (window_->is_Resized()){
        recreateSwapChain(); 
    }

	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK_RESULT(vkWaitForFences(device_->getDevice(), 1, &_renderFence[_currentFrame], true, 1000000000) );
	VK_CHECK_RESULT(vkResetFences(device_->getDevice(), 1, &_renderFence[_currentFrame]) );

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK_RESULT(vkResetCommandBuffer(_mainCommandBuffer[_currentFrame], /*VkCommandBufferResetFlagBits*/ 0));

	//request image from the swapchain
	VkResult  result = vkAcquireNextImageKHR(device_->getDevice(), 
                                            swapchain_->getSwapchain(), 1000000000, 
                                            _presentSemaphore[_currentFrame], 
                                            nullptr,&swapchainImageIndex_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR ) {
        spdlog::error("VK_ERROR_OUT_OF_DATE_KHR");
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK_RESULT(vkBeginCommandBuffer(_mainCommandBuffer[_currentFrame], &cmdBeginInfo)); 

    //initialize the viewport
    VkViewport viewport = vkinit::viewport(swapchain_->getExtent(), 0.0f, 1.0f);
    VkRect2D scissor = vkinit::rect2D(swapchain_->getExtent(), 0, 0);
    vkCmdSetViewport(_mainCommandBuffer[_currentFrame], 0, 1, &viewport);
    vkCmdSetScissor(_mainCommandBuffer[_currentFrame], 0, 1, &scissor);   

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
	VkPresentInfoKHR presentInfo = vkinit::present_info();

    VkSwapchainKHR swapChains[]     = {swapchain_->getSwapchain()};
	presentInfo.pSwapchains         = swapChains;
	presentInfo.pWaitSemaphores     = &_renderSemaphore[_currentFrame];
	presentInfo.pImageIndices       = &swapchainImageIndex_;

    VkResult  result = vkQueuePresentKHR(device_->getPresentQueue(), &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){              
        spdlog::error("VK_ERROR_OUT_OF_DATE_KHR || VK_SUBOPTIMAL_KHR");
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
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

            draw_objects(_mainCommandBuffer[_currentFrame], swapchainImageIndex_);
            draw_fixed(_mainCommandBuffer[_currentFrame], swapchainImageIndex_);
            draw_UiOverlay(_mainCommandBuffer[_currentFrame], swapchainImageIndex_);

    end_renderpass();
    end_frame();

}

void VulkanEngine::draw_objects(VkCommandBuffer cmd, uint32_t imageIndex)
{
    // for(  auto & ro : _renderables){

        RenderObject & ro                   = *renderables_.at(model_index_);
        VulkanShader &shader                = static_cast<VulkanShader&>(Engine::getShader(ro.shader));
        VulkanVertexBuffer &vertexbuffer    = static_cast<VulkanVertexBuffer&>(ro);

        UniformBufferObject mvp = Engine::getMVP();        
        mvp.model = ro.model;
        shader.updateUbo(mvp);
        
        shader.bind(cmd, imageIndex);
        vertexbuffer.draw(cmd, imageIndex);
    // }   
}

void VulkanEngine::draw_fixed(VkCommandBuffer cmd, uint32_t imageIndex)
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

    shader.bind(cmd, imageIndex);
    vertexbuffer.draw(cmd, imageIndex);       
}

void VulkanEngine::draw_UiOverlay(VkCommandBuffer cmd, uint32_t imageIndex)
{
    if(!ui_Overlay_){
        return;
    }

    // feed inputs to dear imgui, start new frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

        Engine::draw_UiOverlay();

    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);
}
