#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

//std
#include <string>

class VulkanImage
{
public:
    VulkanImage(VulkanDevice &device, VulkanSwapchain &swapchain) ;
    ~VulkanImage();

    VkImageView& getTextureImageView() { return textureImageView; }
    VkSampler& getTextureSampler() { return textureSampler; }

private:

    void createTexture();
    void createTextureImageView();
    void createTextureSampler();

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout); 
    void generateMipmaps(VkImage image,const VkFormat imageFormat,  int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    
    VulkanDevice &device;
    VulkanSwapchain &swapchain;

    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageView textureImageView;
    VkSampler textureSampler;
};

