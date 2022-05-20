#pragma once

#include "../Engine.hpp"
#include "vktypes.h"
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
class VulkanUbo;

class VulkanEngine : public Engine
{
public:
    VulkanEngine(EngineType type);
    ~VulkanEngine();

    void draw() override;

private:

    // -----------------------
    void init();
    void init_UiOverlay();
    void init_fixed();
    void init_renderables();
    void init_commands();
    void init_sync_structures();

    void cleanup_UiOverlay();

    void draw_objects(VkCommandBuffer cmd, uint32_t imageIndex);  
    void draw_fixed(VkCommandBuffer cmd, uint32_t imageIndex); 
    void draw_UiOverlay(VkCommandBuffer cmd, uint32_t imageIndex);

    void updateUbo(VulkanUbo &ubo);
    void recreateSwapChain();

    /**
     * @brief Get the Shader object from shaders collection
     * 
     * @param name shader name
     * @return VulkanShader& 
     */
    VulkanShader & getShader(std::string name); 

    // -----------------------
    std::unique_ptr<VulkanDevice> device_;
    std::unique_ptr<VulkanSwapchain> swapchain_;

    //------------------------------------
    int _currentFrame {0};
    const int MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> _presentSemaphore;
    std::vector<VkSemaphore> _renderSemaphore;
	std::vector<VkFence> _renderFence;
    std::vector<VkCommandPool> _commandPool;
	std::vector<VkCommandBuffer> _mainCommandBuffer;

    //------------------------------------
    DeletionQueue _mainDeletionQueue;

    //------------------------------------
    // GUI globals
    VkDescriptorPool _gui_DescriptorPool = VK_NULL_HANDLE; 

};


