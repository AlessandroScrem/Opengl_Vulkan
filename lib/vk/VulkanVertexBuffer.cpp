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
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    // Memory allocation
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.getDevice(), vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, 
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(device.getDevice(), vertexBuffer, vertexBufferMemory, 0);

    // Filling the vertex buffer
    void* data;
    vkMapMemory(device.getDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferInfo.size);
    vkUnmapMemory(device.getDevice(), vertexBufferMemory);
}
