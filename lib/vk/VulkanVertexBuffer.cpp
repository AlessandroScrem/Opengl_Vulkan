#include "VulkanVertexBuffer.hpp"


VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice &device) 
    : device{device}
{ 
    std::cout << "VulkanVertexBuffer  constructor\n"; 
    createVertexBuffer();   
}

VulkanVertexBuffer::~VulkanVertexBuffer() 
{
    std::cout << "VulkanVertexBuffer  destructor\n";
    vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
    vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);  
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
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
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
