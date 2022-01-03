#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"

class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanPipeline &pipeline);
    ~VulkanCommandBuffer();

    // TODO: da verificare funzionamento...
    const VkCommandBuffer & getCommandBuffer(size_t index) const { return (commandBuffers[index]);}

private:
    void createCommandPool();
    void createCommandBuffers();

    VulkanDevice &device;
    VulkanSwapchain &swapchain;
    VulkanPipeline &pipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

};
