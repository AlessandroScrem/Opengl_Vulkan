#include "VulkanDevice.hpp"
#include "VulkanUbo.hpp"
#include "vk_initializers.h"

VulkanUbo::VulkanUbo(VulkanDevice &device) : device{device}
{
    SPDLOG_TRACE("constructor");
    create();
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


void VulkanUbo::create() 
{
    SPDLOG_TRACE("createUniformBuffers");

    VkDeviceSize buffersize = sizeof(UniformBufferObject);
    VkBufferCreateInfo bufferInfo = vkinit::bufferCreateInfo(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, buffersize );

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    device.createVmaBuffer(bufferInfo, vmaallocInfo, uniformBuffer._buffer, uniformBuffer._allocation, static_cast<UniformBufferObject*>(this), buffersize);

}

void VulkanUbo::map() 
{
    device.mapVmaBuffer(uniformBuffer._allocation, static_cast<UniformBufferObject*>(this), sizeof(UniformBufferObject));
}
