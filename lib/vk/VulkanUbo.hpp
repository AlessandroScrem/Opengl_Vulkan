#pragma once
#include <vertex.h>

class VulkanDevice;


class VulkanUbo 
{
public:
   VulkanUbo(VulkanDevice &device, VkDeviceSize size, const void* data);
    ~VulkanUbo();
    void create(const void* data);
    void cleanup();
    void map(const void* data);
    void flush();
    void setDescriptorRange(size_t range){ descriptor.range = range; }


    const VkBuffer& getUniformBuffer(){ return uniformBuffer._buffer;}
    VkDescriptorBufferInfo* getDescriptor(){ return &descriptor; }
private:

    void setupDescriptor();

    VulkanDevice &device;
    VkDeviceSize bufferSize;
    AllocatedBuffer  uniformBuffer;
    VkDescriptorBufferInfo descriptor;
};
