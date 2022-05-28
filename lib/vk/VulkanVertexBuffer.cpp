#include "VulkanDevice.hpp"
#include "VulkanVertexBuffer.hpp"
#include "vk_initializers.h"
//common lib
#include <vertex.h>
#include <model.hpp>

VulkanObjectBuilder::VulkanObjectBuilder(VulkanDevice &device)
: device{device}
{
}

ObjectBuilder& VulkanObjectBuilder::Reset(){
    this->renderobject = std::make_unique<VulkanVertexBuffer>(device);
    return *this;
}

std::unique_ptr<RenderObject> 
VulkanObjectBuilder::build(Model &model, std::string shadername)
{
    this->renderobject->shader = shadername;
    this->renderobject->model = model.get_tranform();
    this->renderobject->build(model);
    std::unique_ptr<RenderObject> result = std::move(this->renderobject);
    return result;
}

VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice &device) : device{device}
{ 
    SPDLOG_DEBUG("constructor");
}

void VulkanVertexBuffer::build(Model &model) 
{ 
    createIndexBuffer(model);   
    createVertexBuffer(model); 

    prepared = true;  
}

VulkanVertexBuffer::~VulkanVertexBuffer() 
{   
    SPDLOG_DEBUG("destructor");
    if(prepared){
        device.destroyVmaBuffer(vertexBuffer._buffer, vertexBuffer._allocation);
        SPDLOG_TRACE("Vertex vmaDestroyBuffer");
        device.destroyVmaBuffer(indexBuffer._buffer, indexBuffer._allocation);
        SPDLOG_TRACE("Index vmaDestroyBuffer");
    }
} 


void VulkanVertexBuffer::createVertexBuffer(Model &model)
{
    //create bufferinfo
    size_t buffersize = static_cast<uint32_t>(sizeof(Vertex) * model.verticesSize());
    const void * bufferdata = model.verticesData();
    VkBufferCreateInfo bufferInfo = vkinit::vertex_input_state_create_info(buffersize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	//allocate the buffer
	device.createVmaBuffer(bufferInfo, vmaallocInfo, vertexBuffer._buffer, vertexBuffer._allocation, bufferdata, buffersize);
}

void VulkanVertexBuffer::createIndexBuffer(Model &model)
{
    //record to local data
    this->indices_size =  model.indicesSize();

    //create bufferinfo
    size_t buffersize = static_cast<uint32_t>(sizeof(Index) * indices_size);
    const void * bufferdata = model.indicesData();
    VkBufferCreateInfo bufferInfo = vkinit::vertex_input_state_create_info(buffersize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	//allocate the buffer
	device.createVmaBuffer(bufferInfo, vmaallocInfo, indexBuffer._buffer, indexBuffer._allocation, bufferdata, buffersize);
}

void VulkanVertexBuffer::draw(VkCommandBuffer cmd, uint32_t imgeIndex)
{
    if(!prepared){
        return;
    }
    VkDeviceSize offsets{};
    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer._buffer, &offsets);
    vkCmdBindIndexBuffer(cmd, indexBuffer._buffer, 0, VK_INDEX_TYPE_UINT32);       
    vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indices_size), 1, 0, 0, 0);   
}
