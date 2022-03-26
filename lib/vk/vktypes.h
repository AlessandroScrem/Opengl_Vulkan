#pragma once
//lib
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct AllocatedBuffer {
	VkBuffer _buffer;
	VmaAllocation _allocation;
};


//we want to immediately abort when there is an error. 
//In normal engines this would give an error message to the user, or perform a dump of state.
 #define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			throw std::runtime_error("Detected Vulkan error: " + std::to_string(err) );       \
		}                                                           \
	} while (0)
 