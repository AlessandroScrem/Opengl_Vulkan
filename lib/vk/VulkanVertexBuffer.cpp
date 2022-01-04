#include "VulkanVertexBuffer.hpp"

/* // Helper funcion
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
*/
VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice &device) 
    : device{device}
{ 
    std::cout << "VulkanVertexBuffer  constructor\n"; 
    createVertexBuffer();   
    std::cout << "VulkanVertexBuffer  createVertexBuffer\n"; 
}

VulkanVertexBuffer::~VulkanVertexBuffer() 
{
    std::cout << "VulkanVertexBuffer  destructor\n";
    vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
    std::cout << "VulkanVertexBuffer  vkDestroyBuffer\n";
    vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);  
    std::cout << "VulkanVertexBuffer  vkFreeMemory\n";
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
void VulkanVertexBuffer::createVertexBuffer() 
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    device.createBuffer(bufferSize,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                        vertexBuffer, 
                        vertexBufferMemory
    );

    // For filling the vertices data to the memory
    // we need a pointer to the buffer which the memory is associated to
    // at the end we unmap a previously mapped memory object 
    void* data;
    vkMapMemory(device.getDevice(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(device.getDevice(), vertexBufferMemory);

}



/* void VulkanVertexBuffer::createVertexBuffer() 
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

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

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
    vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}
 */