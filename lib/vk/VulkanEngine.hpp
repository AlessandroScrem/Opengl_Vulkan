#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanUbo.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanShader.hpp"
#include "../Engine.hpp"
//common lib
#include <baseclass.hpp>
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


class VulkanEngine : public Engine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    void run() override;
    void setWindowMessage(std::string msg) override{window.setWindowMessage(msg);}

private:

    // -----------------------
    void initGUI();
    void init_shaders();
    void init_fixed();
    void init_renderables();
    void init_commands();
    void init_sync_structures();

    void cleanup_GUI();

    void draw();
    void draw_objects(VkCommandBuffer cmd, uint32_t imageIndex);  
    void draw_fixed(VkCommandBuffer cmd, uint32_t imageIndex); 
    void draw_overlay(VkCommandBuffer cmd, uint32_t imageIndex);

    void updateUbo(VulkanUbo &ubo);
    void recreateSwapChain();

    VulkanShader & getShader(std::string name) {
        auto got = _shaders.find (name);
        if ( got == _shaders.end() ){
            throw std::runtime_error("failed to find shader!");
        }
        return static_cast<VulkanShader&>(*got->second);
    }   

    // -----------------------
    Window window{EngineType::Vulkan, Engine::input_};  
    VulkanDevice device{window};
    VulkanSwapchain swapchain{device, window};
    // VulkanImage vulkanimage{device};
    //VulkanUbo global_ubo{device, swapchain};

    //------------------------------------


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
    const bool _overlay = true;
    VkDescriptorPool _gui_DescriptorPool = VK_NULL_HANDLE; 

};


