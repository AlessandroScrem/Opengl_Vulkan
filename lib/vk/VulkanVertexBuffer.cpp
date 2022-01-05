#include "VulkanVertexBuffer.hpp"


VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice &device) 
    : device{device}
{ 
    SPDLOG_TRACE("constructor");
    createIndexBuffer();   
    SPDLOG_TRACE("createIndexBuffer");
    createVertexBuffer();   
    SPDLOG_TRACE("createVertexBuffer");
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