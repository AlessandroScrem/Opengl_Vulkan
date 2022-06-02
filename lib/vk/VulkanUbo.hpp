#pragma once
#include <vertex.h>

class VulkanDevice;


class VulkanUbo : public  UniformBufferObject
{
public:
   VulkanUbo(VulkanDevice &device);
    ~VulkanUbo();
    void create();
    void cleanup();
    void map();

    const VkBuffer& getUniformBuffer(){return uniformBuffer._buffer;}
private:

    VulkanDevice &device;

    // VkBuffer uniformBuffer;
    // VkDeviceMemory uniformBuffersMemory;

    AllocatedBuffer  uniformBuffer;
};
