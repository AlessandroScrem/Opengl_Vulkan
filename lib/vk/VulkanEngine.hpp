#pragma once
#include "Engine.hpp"
#include "common/Window.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanCommandBuffer.hpp"


class VulkanEngine : public Engine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    void run() override;

private:
    void drawFrame();

    void createSyncObjects();

    void recreateSwapChain();
    

    Window window{EngineType::Vulkan};
    
    VulkanDevice device{window};
    VulkanSwapchain swapchain{device, window};
    VulkanPipeline pipeline{device, swapchain};
    VulkanVertexBuffer vertexBuffer{device};
    VulkanCommandBuffer commandBuffer{device, swapchain, pipeline, vertexBuffer};

    

    //  GPU-GPU synchronization
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    //  CPU-GPU synchronization
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    size_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;
};


