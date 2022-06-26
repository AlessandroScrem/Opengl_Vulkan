#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "vk_initializers.h"

//lib
#include <stb_image.h>

VulkanImage::VulkanImage(VulkanDevice &device,  std::string imagepath /*  = "data/textures/viking_room.png" */) 
    : device{device}
    , texpath{imagepath} 
{
    SPDLOG_DEBUG("constructor");
    createTexture();
    createTextureImageView();   
    createTextureSampler();
    setupDescriptor();
}

VulkanImage::~VulkanImage() 
{
    SPDLOG_DEBUG("destructor");
    vkDestroySampler(device.getDevice(), textureSampler, nullptr);
    SPDLOG_TRACE("vkDestroy textureSampler");

    vkDestroyImageView(device.getDevice(), textureImageView, nullptr);
    SPDLOG_TRACE("vkDestroy textureImageView");

    device.destroyVmaImage(textureImage._image, textureImage._allocation);   
    SPDLOG_TRACE("vkDestroy textureImageMemory");
}


void VulkanImage::createTexture() 
{
    SPDLOG_TRACE("createTexture");

    // Adding a texture to our application will involve the following steps:
    // 1) Create an image object backed by device memory
    // 2) Fill it with pixels from an image file
    // 3) Create an image sampler
    // 4) Add a combined image sampler descriptor to sample colors from the texture

    int texWidth, texHeight, texChannels;
    int num_channels = STBI_rgb_alpha;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* pixels = stbi_load(texpath.c_str(), &texWidth, &texHeight, &texChannels, num_channels);

    VkDeviceSize imageSize = texWidth * texHeight * num_channels;

    // calculate number of mipmap 
    // The log2 function calculates how many times that dimension can be divided by 2. 
    // The floor function handles cases where the largest dimension is not a power of 2. 
    // 1 is added so that the original image has a mip level.
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    	
    //create bufferinfo
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
   	
    VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    //allocate temporary buffer for holding texture data to upload
	AllocatedBuffer stagingBuffer{};
    device.createVmaBuffer(bufferInfo, vmaallocInfo, stagingBuffer._buffer, stagingBuffer._allocation, pixels, imageSize );

    // Free up the original pixel array now:
    stbi_image_free(pixels);


    VkExtent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth = 1;

    VkImageCreateInfo img_info = vkinit::image_create_info(
        VK_FORMAT_R8G8B8A8_UNORM, 
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        imageExtent,
        VK_SAMPLE_COUNT_1_BIT, 
        mipLevels);
    
    VmaAllocationCreateInfo img_allocinfo = {};
	img_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    device.createVmaImage(img_info, img_allocinfo, textureImage._image, textureImage._allocation );

    // In orger to generate mipmaps
    // we intend to use the texture image as both the source and destination of a transfer

    // steps
    // 1) Transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    transitionImageLayout(textureImage._image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    // 2) Execute the buffer to image copy operation
    copyBufferToImage(stagingBuffer._buffer, textureImage._image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    
    // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
    // This will leave each level of the texture image in VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
    //  Each level will be transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL after the blit command reading from it is finished.

    // If not mipmap are generated,to be able to start sampling from the texture image in the shader, 
    //   we need one last transition to prepare it for shader access:
    // transitionImageLayout(textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // generateMipmaps(textureImage._image, VK_FORMAT_R8G8B8A8_UNORM,  texWidth, texHeight, mipLevels);
    generateMipmaps(textureImage._image, VK_FORMAT_R8G8B8A8_UNORM,  texWidth, texHeight, mipLevels);

    // cleaning up the staging buffer
    device.destroyVmaBuffer(stagingBuffer._buffer, stagingBuffer._allocation);
}

void VulkanImage::createTextureImageView() 
{
    SPDLOG_TRACE("createTextureImageView");

    VkImageViewCreateInfo viewInfo = vkinit::imageview_create_info(
        VK_FORMAT_R8G8B8A8_UNORM, 
        textureImage._image, 
        VK_IMAGE_ASPECT_COLOR_BIT, 
        mipLevels);

	//create a image-view for the texture image to use for rendering
    VK_CHECK_RESULT(vkCreateImageView(device.getDevice(), &viewInfo, nullptr, &textureImageView));
}

void VulkanImage::createTextureSampler() 
{
    SPDLOG_TRACE("createTextureSampler");
    
    // You can either query the properties at the beginning of your program 
    // and pass them around to the functions that need them, 
    // or query them in the createTextureSampler function itself.
    VkPhysicalDeviceProperties properties = device.getPhysicalDeviceProperties(); 

    auto samplerInfo = vkinit::samplerCreateInfo();
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR; 
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; 
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0.0f;

    VK_CHECK_RESULT(vkCreateSampler(device.getDevice(), &samplerInfo, nullptr, &textureSampler));

}

// move image to be in the right layout
void VulkanImage::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) 
{
    VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

    auto barrier = vkinit::imageMemoryBarrier();
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    // There are two transitions we need to handle:
    // Undefined → transfer destination: transfer writes that don’t need to wait on anything
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
     // Transfer destination → shader reading: shader reads should wait on transfer writes, 
     //  specifically the shader reads in the fragment shader.
     else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        spdlog::error("unsupported layout transition! file {} line  {}", __FILE__, __LINE__);
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    device.endSingleTimeCommands(commandBuffer);
}

void VulkanImage::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) 
{
    VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    device.endSingleTimeCommands(commandBuffer);
}


void VulkanImage::generateMipmaps(VkImage image, const VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
 
    SPDLOG_TRACE("generateMipmaps");
 
    if (!device.findSupportedFormat({imageFormat}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)){
        spdlog::error("texture image format does not support linear blitting!");
    }
    
    VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

    auto barrier = vkinit::imageMemoryBarrier();
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    // This loop will record each of the VkCmdBlitImage commands. 
    // Note that the loop variable starts at 1, not 0.
    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        // First, we transition level i - 1 to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL.
        // This transition will wait for level i - 1 to be filled, either from the previous blit command, or from vkCmdCopyBufferToImage. 
        // The current blit command will wait on this transition.
        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);   

        // Next, we specify the regions that will be used in the blit operation. 
        // The source mip level is i - 1 and the destination mip level is i. 
        // The two elements of the srcOffsets array determine the 3D region that data will be blitted from. 
        // dstOffsets determines the region that data will be blitted to. 
        // The X and Y dimensions of the dstOffsets[1] are divided by two since each mip level is half the size of the previous level. 
        // The Z dimension of srcOffsets[1] and dstOffsets[1] must be 1, since a 2D image has a depth of 1.
        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;


        // Now, we record the blit command. Note that textureImage is used for both the srcImage and dstImage parameter. 
        // The last parameter allows us to specify a VkFilter to use in the blit. 
        // We have the same filtering options here that we had when making the VkSampler.
        // We use the VK_FILTER_LINEAR to enable interpolation.
        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        // This barrier transitions mip level i - 1 to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL. 
        // This transition waits on the current blit command to finish. 
        // All sampling operations will wait on this transition to finish.
        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        // At the end of the loop, we divide the current mip dimensions by two. 
        // We check each dimension before the division to ensure that dimension never becomes 0. 
        // This handles cases where the image is not square, since one of the mip dimensions would reach 1 before the other dimension. 
        // When this happens, that dimension should remain 1 for all remaining levels.
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    // Before we end the command buffer, we insert one more pipeline barrier. 
    // This barrier transitions the last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
    // This wasn’t handled by the loop, since the last mip level is never blitted from.

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);


    device.endSingleTimeCommands(commandBuffer);
}

void VulkanImage::setupDescriptor()
{
		descriptor.sampler = textureSampler;
		descriptor.imageView = textureImageView;
		descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}