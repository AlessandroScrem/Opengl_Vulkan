#include "VulkanEngine.hpp"
#include "vk_initializers.h"
//std
#include <vector>

VulkanEngine::VulkanEngine()
{  
    SPDLOG_TRACE("constructor");

    shaders.emplace_back(std::make_unique<VulkanShader>(device, Engine::phong_glslShader) );
    shaders.emplace_back(std::make_unique<VulkanShader>(device, Engine::tex_glslShader) );
    SPDLOG_TRACE("fine costruzione shaders");    

    auto  &sh0 = *shaders.at(0);
    auto  &sh1 = *shaders.at(1);

    pipelines.emplace_back( std::make_unique<VulkanPipeline>(device, swapchain, vertexbuffer, sh0) );
    pipelines.emplace_back( std::make_unique<VulkanPipeline>(device, swapchain, vertexbuffer, sh1) );
    SPDLOG_TRACE("fine costruzione pipelines");    


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

// void VulkanEngine::upload_models()
// {
//     VulkanVertexBuffer vb{device, swapchain, ubo, vulkanimage, Engine::model};
//     _vertexbuffers["vikingroom"] = &vb;  
// }

// void VulkanEngine::create_defaultPipeline()
// {
//     VulkanVertexBuffer &vb = *get_vertexBuffer("vikingroom");
// }

// Necessita:
// swapchain.getSwapchain
// device getGraphicsQueue
// device.getPresentQueue()
/* 
void VulkanEngine::drawFrame() 
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


/* 
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
        VK_CHECK(vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) );
        VK_CHECK(vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) );
        VK_CHECK(vkCreateFence(device.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) ); 
    }
}

 */
// TODO
//  
// The disadvantage of this approach is that we need to stop all rendering before creating the new swap chain.
// 
// It is possible to create a new swap chain while drawing commands 
// on an image from the old swap chain are still in-flight. 
// You need to pass the previous swap chain to the oldSwapChain field 
// in the VkSwapchainCreateInfoKHR struct and destroy the old swap chain as soon as you’ve finished using it.

// Necessita:
// window.GetWindowExtents()
// pipeline.cleanupPipeline();
// pipeline.createPipeline();
// swapchain.cleanupSwapChain();
// swapchain.createAllSwapchian();
void VulkanEngine::recreateSwapChain() 
{  
     while (window.waitforSize()) {
        window.GetWindowExtents();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.getDevice());

// destroy
    for (auto  & element : pipelines)
    { element->cleanupPipeline ();}

    ubo.cleanupUniformBuffers();
    vertexbuffer.cleanupDescriptorPool();
    swapchain.cleanupSwapChain();

// create
    swapchain.createAllSwapchian();
    ubo.createUniformBuffers();
    vertexbuffer.createDescriptorPool();
    vertexbuffer.createDescriptorSets();
        for (auto & element : pipelines)
    { element->createPipeline ();}
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
/*void VulkanEngine::createCommandBuffers() 
{
    commandBuffers.resize(swapchain.getFramebuffersSize());

    VkCommandPool commandPool = device.getCommadPool();
    uint32_t commandBufferCount = (uint32_t) commandBuffers.size();
    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(commandPool, commandBufferCount);

    VK_CHECK( vkAllocateCommandBuffers(device.getDevice(), &cmdAllocInfo, commandBuffers.data()) );

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = vkinit::command_buffer_begin_info() ;

        VK_CHECK(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) );

            VkRenderPassBeginInfo renderPassInfo = vkinit::renderpass_begin_info(
                swapchain.getRenderpass(), swapchain.getExtent(), swapchain.getFramebuffer(i));

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

        VK_CHECK(vkEndCommandBuffer(commandBuffers[i]));
    }
}
*/

/*void VulkanEngine::cleanupCommandBuffers()
{
    vkFreeCommandBuffers(device.getDevice(), 
                        device.getCommadPool(), 
                        static_cast<uint32_t>(commandBuffers.size()), 
                        commandBuffers.data());    
}
*/

/* Material* VulkanEngine::create_material(VkPipeline pipeline, VkPipelineLayout pipelineLayout, ShaderType type, std::string& name)
{
    Material mat;
    VulkanShader sh{device, type};

	mat.shader = &sh;
	mat.pipeline = pipeline;
	mat.pipelineLayout = pipelineLayout;
	_materials[name] = mat;
	return &_materials[name];
}

Material* VulkanEngine::get_material(const std::string& name)
{
	//search for the object, and return nullptr if not found
	auto it = _materials.find(name);
	if (it == _materials.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}

VulkanVertexBuffer* VulkanEngine::get_vertexBuffer(const std::string& name)
{
	auto it = _vertexbuffers.find(name);
	if (it == _vertexbuffers.end()) {
		return nullptr;
	}
	else {
		return (*it).second;
	}
}

 */


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

        VulkanPipeline &pipeline = *pipelines.at(_selectedShader);

	    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());

        ubo.bind(0);

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

