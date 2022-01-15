#include "VulkanVertexBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"




VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanImage &vulkanimage) 
    : device{device}
    ,swapchain{swapchain}
    ,vulkanimage{vulkanimage}
{ 
    SPDLOG_TRACE("constructor");
    createIndexBuffer();   
    createVertexBuffer();   
    createDescriptorSetLayout();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

VulkanVertexBuffer::~VulkanVertexBuffer() 
{   
    SPDLOG_TRACE("destructor");

    vkDestroyBuffer(device.getDevice(), indexBuffer, nullptr);
    SPDLOG_TRACE("Index vkDestroyBuffer");
    vkFreeMemory(device.getDevice(), indexBufferMemory, nullptr);
    SPDLOG_TRACE("Index vkFreeMemory");
    vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
    SPDLOG_TRACE("Vertex vkDestroyBuffer");
    vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);  
    SPDLOG_TRACE("Vertex vkFreeMemory");

    cleanupUniformBuffers();
    cleanupDescriptorPool();

    vkDestroyDescriptorSetLayout(device.getDevice(), descriptorSetLayout, nullptr);
    SPDLOG_TRACE("vkDestroyDescriptorSetLayout");
} 

// Necessita
// swapchain.getSwapchianImageSize()
void VulkanVertexBuffer::cleanupUniformBuffers() 
{   
    SPDLOG_TRACE("cleanupUniformBuffers");

    auto swapchainImages =  swapchain.getSwapchianImageSize();
    for (size_t i = 0; i < swapchainImages ; i++) {
        vkDestroyBuffer(device.getDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(device.getDevice(), uniformBuffersMemory[i], nullptr);
    }
}

void VulkanVertexBuffer::cleanupDescriptorPool()
{
    SPDLOG_TRACE("cleanupDescriptorPool");

    vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
}


/*
    Unfortunately the driver may not immediately copy the data into the buffer memory, for example because of caching. 
    It is also possible that writes to the buffer are not visible in the mapped memory yet. 
    There are two ways to deal with that problem:
    
    - Use a memory heap that is host coherent, indicated with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    
    - Call vkFlushMappedMemoryRanges after writing to the mapped memory, 
        and call vkInvalidateMappedMemoryRanges before reading from the mapped memory

    Flushing memory ranges or using a coherent memory heap means that the driver will be aware of our writes to the buffer,
    but it doesn’t mean that they are actually visible on the GPU yet. 
    The transfer of data to the GPU is an operation that happens in the background and the specification 
    simply tells us that it is guaranteed to be complete as of the next call to vkQueueSubmit.
*/
// Necessita
// device.findMemoryType
// device.copyBuffer
void VulkanVertexBuffer::createVertexBuffer() 
{
    SPDLOG_TRACE("createVertexBuffer");

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Using a staging buffer
    // We’re now going to change createVertexBuffer to only use a host visible buffer 
    // as temporary buffer and use a device local one as actual vertex buffer.
    // Vertex data will be loaded from high performance memory
    device.createBuffer(bufferSize, 
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                        stagingBuffer, 
                        stagingBufferMemory
    );

    // For filling the vertices data to the memory
    // we need a pointer to the buffer which the memory is associated to
    // at the end we unmap a previously mapped memory object 
    void* data;
    vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(device.getDevice(), stagingBufferMemory);

    // The vertexBuffer is now allocated from a memory type that is device local
    // which generally means that we’re not able to use vkMapMemory. 
    // However, we can copy data from the stagingBuffer to the vertexBuffer
    device.createBuffer(bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                        vertexBuffer, 
                        vertexBufferMemory
    );

    device.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
    vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

void VulkanVertexBuffer::createIndexBuffer() 
{
    SPDLOG_TRACE("createIndexBuffer");

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                        stagingBuffer, 
                        stagingBufferMemory
                        );

    void* data;
    vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(device.getDevice(), stagingBufferMemory);

    device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                        indexBuffer, 
                        indexBufferMemory
                        );

    device.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
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

// Necessita
// swapchain.getSwapchianImageSize()
void VulkanVertexBuffer::createUniformBuffers() 
{
    SPDLOG_TRACE("createUniformBuffers");

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    auto swapchainImages =  swapchain.getSwapchianImageSize();
    uniformBuffers.resize(swapchainImages);
    uniformBuffersMemory.resize(swapchainImages);

    for (size_t i = 0; i < swapchainImages; i++) {
        device.createBuffer(bufferSize, 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            uniformBuffers[i], 
            uniformBuffersMemory[i]);
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
void VulkanVertexBuffer::createDescriptorSets() 
{
    SPDLOG_TRACE("createDescriptorSets");


    auto swapchainImages =  swapchain.getSwapchianImageSize();

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

void VulkanVertexBuffer::updateUniformBuffer(uint32_t currentImage) 
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


    UniformBufferObject ubo{};
 //   ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
 //   ubo.model = glm::scale(ubo.model, glm::vec3{2.0});
//    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//    ubo.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    auto extent = swapchain.getExtent();
    ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float) extent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(device.getDevice(), uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device.getDevice(), uniformBuffersMemory[currentImage]);
}

