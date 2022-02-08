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
  - createColorResources();
  - createDepthResources();
  - createFramebuffers();
- VulkanUbo
  - createUniformBuffers()

- VulkanImage
  - createTextureImage();
  - createTextureImageView();
  - createTextureSampler();

- VulkanVertexBuffer
  - createVertexBuffer();
  - createIndexBuffer();
  - createDescpriptorSetLayout()
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
  - cleanupDescriptorPool()
  - destroyDescpriptorSetLayout()

- VulkanImage
  - vkDestroy textureSampler
  - vkDestroy ImageView
  - vkDestroy textureImage
  - vkFree textureImageMemory

- VulkanUbo
  - cleanupUniformBuffers()

- VulkanSwapchian
  - vkDestroyFramebuffer()
  - vkDestroy ColorResources()
  - vkDestroy DepthResources()
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


