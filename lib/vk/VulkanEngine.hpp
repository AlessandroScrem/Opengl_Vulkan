#pragma once
#include "Engine.hpp"
#include "common/Window.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanVertexBuffer.hpp"

class VulkanEngine : public Engine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    void run() override;


private:
    void drawFrame();

    void createSyncObjects();

    // const VkCommandBuffer & getCommandBuffer(size_t index) const { return (commandBuffers[index]);}

    void recreateSwapChain();
    void createCommandBuffers();
    void cleanupCommandBuffers();

    Window window{EngineType::Vulkan};
    
    VulkanDevice device{window};
    VulkanSwapchain swapchain{device, window};
    VulkanVertexBuffer vertexbuffer{device, swapchain};
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


