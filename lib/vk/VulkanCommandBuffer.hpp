#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"

class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanPipeline &pipeline);
    ~VulkanCommandBuffer();

    const VkCommandBuffer & getCommandBuffer(size_t index) const { return (commandBuffers[index]);}

    // used by VulkanEngine
    void cleanupCommandBuffers();
    void createCommandBuffers();

private:
    void createCommandPool();

    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    VulkanPipeline &pipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

};
