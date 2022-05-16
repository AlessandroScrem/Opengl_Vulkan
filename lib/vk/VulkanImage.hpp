#pragma once
//std
#include <string>

class VulkanDevice;

class VulkanImage
{
public:
    VulkanImage(VulkanDevice &device, std::string imagepath = "data/textures/viking_room.png" ) ;
    ~VulkanImage();

    VkImageView& getTextureImageView() { return textureImageView; }
    VkSampler& getTextureSampler() { return textureSampler; }

private:

    void createTexture();
    void createTextureImageView();
    void createTextureSampler();

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout); 
    void generateMipmaps(VkImage image,const VkFormat imageFormat,  int32_t texWidth, int32_t texHeight, uint32_t mipLevels);


    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    
    VulkanDevice &device;
    std::string texpath{};  

    uint32_t mipLevels;

    AllocatedImage textureImage;

    VkImageView textureImageView;
    VkSampler textureSampler;
};

