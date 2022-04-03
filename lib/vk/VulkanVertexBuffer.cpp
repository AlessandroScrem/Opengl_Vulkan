#include "VulkanVertexBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "vk_initializers.h"



VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanUbo &ubo, VulkanImage &vulkanimage, Model &model) 
    : device{device}
    , swapchain{swapchain}
    , ubo{ubo}
    , vulkanimage{vulkanimage}
    , model{model}
{ 
    SPDLOG_DEBUG("constructor");
    createIndexBuffer();   
    createVertexBuffer();   
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
}

VulkanVertexBuffer::~VulkanVertexBuffer() 
{   
    SPDLOG_DEBUG("destructor");

    device.destroyVmaBuffer(vertexBuffer._buffer, vertexBuffer._allocation);
    SPDLOG_TRACE("Vertex vmaDestroyBuffer");
    device.destroyVmaBuffer(indexBuffer._buffer, indexBuffer._allocation);
    SPDLOG_TRACE("Index vmaDestroyBuffer");

    cleanupDescriptorPool();

    vkDestroyDescriptorSetLayout(device.getDevice(), descriptorSetLayout, nullptr);
    SPDLOG_TRACE("vkDestroyDescriptorSetLayout");
} 

void VulkanVertexBuffer::cleanupDescriptorPool()
{
    SPDLOG_TRACE("cleanupDescriptorPool");

    vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
}


void VulkanVertexBuffer::createVertexBuffer()
{
    //create bufferinfo
    size_t buffersize = sizeof(Vertex) * model.verticesSize();
    const void * bufferdata = model.verticesData();
    VkBufferCreateInfo bufferInfo = vkinit::vertex_input_state_create_info(buffersize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	//allocate the buffer
	device.createVmaBuffer(bufferInfo, vmaallocInfo, vertexBuffer._buffer, vertexBuffer._allocation, bufferdata, buffersize);
}

void VulkanVertexBuffer::createIndexBuffer()
{
    //create bufferinfo
    size_t buffersize = sizeof(Index) * model.indicesSize();
    const void * bufferdata = model.indicesData();
    VkBufferCreateInfo bufferInfo = vkinit::vertex_input_state_create_info(buffersize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	//allocate the buffer
	device.createVmaBuffer(bufferInfo, vmaallocInfo, indexBuffer._buffer, indexBuffer._allocation, bufferdata, buffersize);
}
 
// creare prima di createGraphicsPipeline();
void VulkanVertexBuffer::createDescriptorSetLayout()
{
    SPDLOG_TRACE("createDescriptorSetLayout");

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void VulkanVertexBuffer::createDescriptorPool() 
{
    SPDLOG_TRACE("createDescriptorPool");

    auto swapchainImages =  swapchain.getSwapchianImageSize();

    std::array<VkDescriptorPoolSize, 2> poolSize{};
    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize[0].descriptorCount = static_cast<uint32_t>(swapchainImages); 
    poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount = static_cast<uint32_t>(swapchainImages); 

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
    poolInfo.pPoolSizes = poolSize.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapchainImages);

    if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

}

//  You don’t need to explicitly clean up descriptor sets, 
//  because they will be automatically freed when the descriptor pool is destroyed
// Necessita
// swapchain.getSwapchianImageSize();
// vulkanimage.getTextureImageView();
// vulkanimage.getTextureSampler();
// vulkanubo.getUniformBuffers();
void VulkanVertexBuffer::createDescriptorSets() 
{
    SPDLOG_TRACE("createDescriptorSets");


    auto swapchainImages =  swapchain.getSwapchianImageSize();
    auto& uniformBuffers = ubo.getUniformBuffers();

    std::vector<VkDescriptorSetLayout> layouts(swapchainImages, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainImages);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(swapchainImages);
    if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // populate every descriptor:
    for (size_t i = 0; i < swapchainImages; i++) {
 
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = vulkanimage.getTextureImageView();
        imageInfo.sampler =  vulkanimage.getTextureSampler();

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device.getDevice(), 
                static_cast<uint32_t>(descriptorWrites.size()), 
                descriptorWrites.data(), 
                0, nullptr);
    }
}
