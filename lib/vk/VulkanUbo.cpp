#include "VulkanDevice.hpp"
#include "VulkanUbo.hpp"
#include "vk_initializers.h"

VulkanUbo::VulkanUbo(VulkanDevice &device, VkDeviceSize size, const void* data) 
: device{device},
bufferSize{size}
{
    SPDLOG_TRACE("constructor");
    create(data);
    setupDescriptor();
}

VulkanUbo::~VulkanUbo()
{
    SPDLOG_TRACE("destructor");
    cleanup();
}

void VulkanUbo::cleanup() 
{   
    SPDLOG_TRACE("cleanupUniformBuffers");
    device.destroyVmaBuffer(uniformBuffer._buffer, uniformBuffer._allocation);
}


void VulkanUbo::create(const void* data) 
{
    SPDLOG_TRACE("createUniformBuffers");

    VkBufferCreateInfo bufferInfo = vkinit::bufferCreateInfo(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, bufferSize );

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    device.createVmaBuffer(bufferInfo, vmaallocInfo, uniformBuffer._buffer, uniformBuffer._allocation, data, bufferSize);
}

void VulkanUbo::map(const void* data) 
{
    device.mapVmaBuffer(uniformBuffer._allocation, data, bufferSize);
}

void VulkanUbo::flush() 
{
    const size_t offset = 0;
    device.flushVmaAllocation(uniformBuffer._allocation, offset, bufferSize);
}

void VulkanUbo::setupDescriptor()
{
        const size_t offset = 0;
    	descriptor.offset = offset;
		descriptor.buffer = uniformBuffer._buffer;
		descriptor.range = bufferSize;
}
