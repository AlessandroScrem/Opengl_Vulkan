#pragma once

#include "VulkanDevice.hpp"

class VulkanImage
{
public:
    VulkanImage(VulkanDevice &device) ;
    ~VulkanImage();

private:

    void createTexture();
    void createTextureImageView();
    void createTextureSampler();

    void createImage(uint32_t width, uint32_t height, 
                    VkFormat format, VkImageTiling tiling, 
                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
                    VkImage& image, VkDeviceMemory& imageMemory);
                    
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout); 

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkImageView createImageView(VkImage image, VkFormat format);
    
    VulkanDevice &device;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageView textureImageView;
    VkSampler textureSampler;
};

