#include "VulkanEngine.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanUbo.hpp"
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
    Shader::addBuilder(std::make_unique<ShaderBuilder>(*device_, *swapchain_));

    Engine::init_shaders(); 
       
    init_fixed();           
    init_renderables();

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
    auto                     g_CheckVkResultFn  = [](VkResult x){ VK_CHECK(x);};            
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
        VK_CHECK(vkCreateDescriptorPool(g_Device, &pool_info, NULL, &_gui_DescriptorPool));
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

void VulkanEngine::init_renderables()
{ 
    SPDLOG_TRACE("init_randerables");

    {
        Model model("data/models/viking_room.obj", Model::UP::ZUP);
        // rotate toward camera
        glm::mat4 trasf = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model.set_transform(trasf);

        std::unique_ptr<RenderObject> object = std::make_unique<VulkanVertexBuffer>(*device_, model);
        object->shader = "texture";
        object->model = model.get_tranform();
        renderables_.push_back(std::move(object));
    }

    {
        Model model("data/models/suzanne.obj", Model::UP::YUP);
        std::unique_ptr<RenderObject> object = std::make_unique<VulkanVertexBuffer>(*device_, model);
        object->shader = "normalmap";
        object->model = model.get_tranform();
        renderables_.push_back(std::move(object));

    }
}

void VulkanEngine::init_fixed()
{
    SPDLOG_TRACE("init_fixed"); 

    {
        std::unique_ptr<RenderObject> object = std::make_unique<VulkanVertexBuffer>(*device_, Model::axis());
        object->shader = "axis";
        object->model = glm::mat4(1.0f);
        fixed_objects_.emplace("axis", std::move(object)); 
    }    

}


VulkanShader & VulkanEngine::getShader(std::string name) 
{
    auto got = shaders_.find (name);
    if ( got == shaders_.end() ){
        throw std::runtime_error("failed to find shader!");
    }
    return static_cast<VulkanShader&>(*got->second);
}  
 
void VulkanEngine::updateUbo(VulkanUbo &ubo)
{

    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window_->getWindowAspect(), 1.0f, 11.0f);
    ubo.proj[1][1] *= -1;

    ubo.viewPos = ourCamera.GetPosition();
} 


void VulkanEngine::recreateSwapChain() 
{  
    SPDLOG_DEBUG("recreateSwapChain");

    vkDeviceWaitIdle(device_->getDevice());

    // destroy 
    //global_ubo.cleanupUniformBuffers();  
    swapchain_->cleanupSwapChain();

    // create 
    swapchain_->createAllSwapchian();
    //global_ubo.createUniformBuffers();

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
        VK_CHECK(vkCreateSemaphore(device_->getDevice(), &semaphoreCreateInfo, nullptr, &_presentSemaphore[i]));
        VK_CHECK(vkCreateSemaphore(device_->getDevice(), &semaphoreCreateInfo, nullptr, &_renderSemaphore[i]));
	    VK_CHECK(vkCreateFence(device_->getDevice(), &fenceCreateInfo, nullptr, &_renderFence[i]));

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
	    VK_CHECK(vkAllocateCommandBuffers(device_->getDevice(), &cmdAllocInfo, &_mainCommandBuffer[i]));

        _mainDeletionQueue.push_function([=]() {
            vkDestroyCommandPool(device_->getDevice(), _commandPool[i], nullptr);
        });
    }
}


void VulkanEngine::draw()
{
    if (window_->is_Resized()){
        recreateSwapChain(); 
    }

	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(device_->getDevice(), 1, &_renderFence[_currentFrame], true, 1000000000) );
	VK_CHECK(vkResetFences(device_->getDevice(), 1, &_renderFence[_currentFrame]) );

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(_mainCommandBuffer[_currentFrame], /*VkCommandBufferResetFlagBits*/ 0));

	//request image from the swapchain
	uint32_t swapchainImageIndex;
	VkResult  result = vkAcquireNextImageKHR(device_->getDevice(), swapchain_->getSwapchain(), 1000000000, 
        _presentSemaphore[_currentFrame], nullptr,&swapchainImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR ) {
        spdlog::error("VK_ERROR_OUT_OF_DATE_KHR");
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

	//naming it cmd for shorter writing
	VkCommandBuffer cmd = _mainCommandBuffer[_currentFrame];
	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	//make a clear-color.
	VkClearValue clearValue;
    // set the background color
    float r = Engine::background.red;
    float g = Engine::background.green;
    float b = Engine::background.blue;
    float a = Engine::background.alpha;
	clearValue.color = { { r, g, b, a } };

	//clear depth at 1
	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.f;

	VkClearValue clearValues[] = { clearValue, depthClear };

	//start the main renderpass. 
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(
        swapchain_->getRenderpass(), 
        swapchain_->getExtent(), 
        swapchain_->getFramebuffer(swapchainImageIndex));

	//connect clear values
	rpInfo.clearValueCount = 2;
	rpInfo.pClearValues = &clearValues[0];

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

        draw_objects(cmd, swapchainImageIndex);
        draw_fixed(cmd, swapchainImageIndex);
        draw_UiOverlay(cmd, swapchainImageIndex);

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
	submit.pWaitSemaphores = &_presentSemaphore[_currentFrame];
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &_renderSemaphore[_currentFrame];

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit(device_->getPresentQueue(), 1, &submit, _renderFence[_currentFrame]));

	//prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = vkinit::present_info();

    VkSwapchainKHR swapChains[]     = {swapchain_->getSwapchain()};
	presentInfo.pSwapchains         = swapChains;
	presentInfo.swapchainCount      = 1;
	presentInfo.pWaitSemaphores     = &_renderSemaphore[_currentFrame];
	presentInfo.waitSemaphoreCount  = 1;
	presentInfo.pImageIndices       = &swapchainImageIndex;

    result = vkQueuePresentKHR(device_->getPresentQueue(), &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){              
        spdlog::error("VK_ERROR_OUT_OF_DATE_KHR || VK_SUBOPTIMAL_KHR");
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

	//next frame 0 -> MAX_FRAMES_IN_FLIGHT -1    
	_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;    
}

void VulkanEngine::draw_objects(VkCommandBuffer cmd, uint32_t imageIndex)
{
    // for(  auto & ro : _renderables){

        RenderObject & ro                   = *renderables_.at(model_index_);
        VulkanShader &shader                = getShader(ro.shader);
        VulkanUbo & ubo                     = shader.getUbo();
        VulkanVertexBuffer &vertexbuffer    = static_cast<VulkanVertexBuffer&>(ro);

        updateUbo(ubo);
        ubo.model = ro.model;

	    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.getGraphicsPipeline()); 
        ubo.bind(imageIndex);

        VkBuffer vertexBuffers[]        = {vertexbuffer.getVertexBuffer()};
        VkBuffer indexBuffer            = vertexbuffer.getIndexBuffer();
        size_t indexsize                = vertexbuffer.getIndexSize();
        VkDeviceSize offsets[1]          = {0};
        VkDescriptorSet descriptorSet   = shader.getDescriptorSet(imageIndex);
        VkPipelineLayout pipelineLayout = shader.getPipelineLayout();

        VkViewport viewport{};
        VkRect2D scissor{};
        scissor.extent = swapchain_->getExtent();
        scissor.offset = {0, 0};
        viewport.y = 0.0f;
        viewport.x = 0.0f;
        viewport.height = static_cast<float>(scissor.extent.height);
        viewport.width  = static_cast<float>(scissor.extent.width);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

  
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);       
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indexsize), 1, 0, 0, 0);
    // }   
}

void VulkanEngine::draw_fixed(VkCommandBuffer cmd, uint32_t imageIndex)
{
        
    RenderObject & ro                   = *fixed_objects_.at("axis");
    VulkanShader & shader               = getShader(ro.shader);
    VulkanUbo & ubo                     = shader.getUbo();
    VulkanVertexBuffer &vertexbuffer    = static_cast<VulkanVertexBuffer&>(ro);
    
    auto[x, y] = window_->extents();
    //set new world origin to bottom left + offset
    float offset = 50; 
    float left   = -offset;
    float right  = x-offset;
    float bottom = y-offset;
    float top    = -offset;

    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::orthoLH_ZO(left, right, bottom, top, -100.0f, 100.0f);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.getGraphicsPipeline());
    ubo.bind(imageIndex);

    VkBuffer vertexBuffers[]        = {vertexbuffer.getVertexBuffer()};
    VkBuffer indexBuffer            = vertexbuffer.getIndexBuffer();
    size_t indexsize                = vertexbuffer.getIndexSize();
    VkDeviceSize offsets[]          = {0};
    VkDescriptorSet descriptorSet   = shader.getDescriptorSet(imageIndex);
    VkPipelineLayout pipelineLayout = shader.getPipelineLayout();


    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);       
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indexsize), 1, 0, 0, 0); 
        
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
