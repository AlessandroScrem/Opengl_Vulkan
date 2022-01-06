# Vulkan

 ##  Constuction order:


- Window
  - glfwCreateWindow()

- VulkanDevice
  - createInstance();
  - setupDebugMessenger();
  - createSurface();
  - pickPhysicalDevice();
  - createLogicalDevice();
  - createCommandPool()

- VulkanSwapchain
  - createSwapChain();
  - createImageViews();
  - createRenderPass();
  - createFramebuffers();

- VulkanVertexBuffer
  - createVertexBuffer();
  - createIndexBuffer();
  - createDescpriptorSetLayout()
  - createUniformBuffers()
  - createDescriptorPool()
  - createDescriptorSets()

- VulkanPipeline
  - createPipeline()
    - pipeLineLayout
    - graphicsPipeline

- VulkanEngine()
  - createCommandBuffer()
  - createSemaphores()

 ## Destruction order:

- VulkanEngine
  - vkDestroyBuffer();
  - vkFreeCommandBuffers()
  - vkDestroySemaphores()
  
- VulakanPipeline
  - vkDestroyPipeline(graphicsPipeline)
  - vkDestroyPipelineLayout(pipeLineLayout)

- VulkanVertexBuffer
  - Index destroyBuffer()
  - Index FreeMemory();
  - Vertex destroyBuffer()
  - Vertex FreeMemory();
  - cleanupUniformBuffers()
  - cleanupDescriptorPool()
  - destroyDescpriptorSetLayout()

- VulkanSwapchian
  - vkDestroyFramebuffer()
  - vkDestroyImageView()
  - vkDestroyRenderPass()
  - vkDestroySwapchainKHR()
  - cleanupSwapChain()

- VulkanDevice       
  - vkDestroyCommandPool()
  - vkDestroyDevice();
  - DestroyDebugUtilsMessengerEXT();
  - vkDestroySurfaceKHR();
  - vkDestroyInstance();

- Window
  - glfwCreateWindow()


