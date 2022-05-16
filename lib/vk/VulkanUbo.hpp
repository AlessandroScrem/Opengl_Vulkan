#pragma once
#include <vertex.h>

class VulkanDevice;


class VulkanUbo : public  UniformBufferObject
{
public:
    VulkanUbo(VulkanDevice &device, size_t sc_images);
    ~VulkanUbo();
    void createUniformBuffers();
    void cleanupUniformBuffers();
    void bind(uint32_t currentImage);

    const std::vector<VkBuffer>& getUniformBuffers(){return uniformBuffers;}
private:

    VulkanDevice &device;
    size_t swapchainImages{};

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
};
