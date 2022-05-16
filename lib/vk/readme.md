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
  - createVulkanAllocator()
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

- VulkanShader
  - buildShaders()
  - createDescpriptorSetLayout()
  - createDescriptorPool()
  - createDescriptorSets()
  - createPipeline()

- VulkanEngine()
  - create _shaders
  - init_renderables()
  - init_commands()
  - init_sync_structures()

 ## Destruction order:

- VulkanEngine
  - destroy _shaders
  - destroy _renderables
  - vkDestroyBuffer();
  - vkFreeCommandBuffers()
  - vkDestroySemaphores()
  
- VulkanShader
  - vkDestroyPipeline(graphicsPipeline)
  - vkDestroyPipelineLayout(pipeLineLayout)
  - cleanupDescriptorPool()
  - destroyDescpriptorSetLayout()
  - vkDestroyShaderModule(vertexmodule)
  - vkDestroyShaderModule(fragmentexmodule)

- VulkanVertexBuffer
  - Index destroyBuffer()
  - Index FreeMemory();
  - Vertex destroyBuffer()
  - Vertex FreeMemory();

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
  - vmaDestroyAllocator()
  - vkDestroyDevice();
  - DestroyDebugUtilsMessengerEXT();
  - vkDestroySurfaceKHR();
  - vkDestroyInstance();

- Window
  - glfwCreateWindow()


