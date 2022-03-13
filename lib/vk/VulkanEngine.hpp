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

class VulkanEngine : public Engine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    void run() override;
    void setWindowMessage(std::string msg) override{window.setWindowMessage(msg);};


private:
    void drawFrame();
    void updateUbo();

    void createSyncObjects();
    void recreateSwapChain();
    void createCommandBuffers();
    void cleanupCommandBuffers();

    Window window{EngineType::Vulkan, Engine::input_};
    
    VulkanDevice device{window};
    VulkanSwapchain swapchain{device, window};
    VulkanUbo ubo{device, swapchain};
    VulkanImage vulkanimage{device, swapchain};
    VulkanVertexBuffer vertexbuffer{device, swapchain, ubo, vulkanimage};
    VulkanShader vulkanshader{device, GLSL::PHONG_SHADER};
    VulkanPipeline pipeline{device, swapchain, vertexbuffer, vulkanshader};

    std::vector<VkCommandBuffer> commandBuffers;

    //  GPU-GPU synchronization
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    //  CPU-GPU synchronization
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    size_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;
};


