#pragma once
//lib
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <string>
//common lib
#include <mytypes.hpp>

struct AllocatedBuffer {
	VkBuffer _buffer;
	VmaAllocation _allocation;
};

struct AllocatedImage {
    VkImage _image;
    VmaAllocation _allocation;
};

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		spdlog::error("Fatal : VkResult is {}  in {}  at line {}", vks::tools::errorString(res) , __FILE__ , __LINE__); \
		assert(res == VK_SUCCESS);																		\
	}																									\
}


namespace vks
{
	namespace tools
	{
		/** @brief Returns an error code as a string */
		std::string errorString(VkResult errorCode);
	}
}

