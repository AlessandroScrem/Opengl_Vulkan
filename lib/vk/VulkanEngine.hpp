#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanUbo.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanImage.hpp"
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
    void setWindowMessage(std::string msg) override{};


private:
    void drawFrame();
    void updateUbo(uint32_t currentImage);

    void createSyncObjects();

    // const VkCommandBuffer & getCommandBuffer(size_t index) const { return (commandBuffers[index]);}

    void recreateSwapChain();
    void createCommandBuffers();
    void cleanupCommandBuffers();

    Window window{EngineType::Vulkan, Engine::input_};
    
    VulkanDevice device{window};
    VulkanSwapchain swapchain{device, window};
    VulkanUbo ubo{device, swapchain};
    VulkanImage vulkanimage{device, swapchain};
    VulkanVertexBuffer vertexbuffer{device, swapchain, ubo, vulkanimage};
    VulkanPipeline pipeline{device, swapchain, vertexbuffer};

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


