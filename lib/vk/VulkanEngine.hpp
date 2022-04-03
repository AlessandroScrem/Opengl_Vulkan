#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanUbo.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanShader.hpp"
#include "../Engine.hpp"
//common lib
#include <Window.hpp>
#include <multiplatform_input.hpp>
#include <unordered_map>
#include <deque>

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call functors
        }

        deletors.clear();
    }
};


struct RenderObject {
	
    std::unique_ptr<VulkanVertexBuffer> vertexbuffer;
	std::unique_ptr<VulkanPipeline>  pipeline;
	VulkanUbo &ubo;
};

const unsigned int MAX_FRAMES_IN_FLIGHT = 2;    

class VulkanEngine : public Engine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    void run() override;
    void setWindowMessage(std::string msg) override{window.setWindowMessage(msg);}

private:

    // -----------------------
    // -----------------------
    void init_renderables();
    void init_commands();
    void init_sync_structures();

    void draw();
    void draw_objects(VkCommandBuffer cmd,RenderObject* first, int count);  
    void updateUbo();
    void recreateSwapChain();   

    Window window{EngineType::Vulkan, Engine::input_};
    
    VulkanDevice device{window};
    VulkanSwapchain swapchain{device, window};
    VulkanUbo ubo{device, swapchain};
    VulkanImage vulkanimage{device, swapchain};

    std::vector<RenderObject> _renderables;
    std::vector<std::unique_ptr<VulkanShader>> _shaders;
    std::vector<std::unique_ptr<VulkanVertexBuffer>> _vertexbuffers;
    std::vector<std::unique_ptr<VulkanPipeline>> _pipelines;
 

    //------------------------------------
    //------------------------------------
    
    VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;
    DeletionQueue _mainDeletionQueue;

    VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

    int _frameNumber {0};

    //------------------------------------
    //------------------------------------

};


