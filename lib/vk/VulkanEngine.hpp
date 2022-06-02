#pragma once

#include "../Engine.hpp"
#include "vktypes.h"
#include "VulkanUIOverlay.h"
//common lib
#include <baseclass.hpp>
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

class VulkanDevice;
class VulkanSwapchain;
class VulkanShader;
class ShaderBuilder;

class VulkanEngine : public Engine
{
public:
    VulkanEngine(EngineType type);
    ~VulkanEngine();

protected:
    void draw() override;
    void resizeFrame() override;

private:

    // -----------------------
    void init();
    void init_commands();
    void init_sync_structures();
    void cleanup();

    void begin_frame();
    void end_frame();
    void begin_renderpass();
    void end_renderpass();

    void draw_objects(VkCommandBuffer cmd);  
    void draw_fixed(VkCommandBuffer cmd); 

    void recreateSwapChain();

    // -----------------------
    std::unique_ptr<VulkanDevice> device_;
    std::unique_ptr<VulkanSwapchain> swapchain_;
    VulkanUIOverlay UIoverlay;

    //------------------------------------
    int _currentFrame {0};
    const int MAX_FRAMES_IN_FLIGHT = 1;
    uint32_t swapchainImageIndex_;
    
    std::vector<VkSemaphore> _presentSemaphore;
    std::vector<VkSemaphore> _renderSemaphore;
	std::vector<VkFence> _renderFence;
    std::vector<VkCommandPool> _commandPool;
	std::vector<VkCommandBuffer> _mainCommandBuffer;

    //------------------------------------
    DeletionQueue _mainDeletionQueue;
};


