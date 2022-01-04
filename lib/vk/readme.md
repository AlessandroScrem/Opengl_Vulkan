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

- VulkanPipeline
  - createPipeline()
    - pipeLineLayout
    - graphicsPipeline

- VulkanVertexBuffer
  - createVertexBuffer();

- VulkanEngine()
  - createCommandBuffer()
  - createSemaphores()

 ## Destruction order:

- VulkanEngine
  - vkDestroyBuffer();
  - vkFreeCommandBuffers()
  - vkDestroySemaphores()
  
- VulkanVertexBuffer

- VulakanPipeline
  - vkDestroyPipeline(graphicsPipeline)
  - vkDestroyPipelineLayout(pipeLineLayout)

- VulkanSwapchian
  - vkDestroyFramebuffer()
  - vkDestroyImageView()
  - vkDestroyRenderPass()
  - vkDestroySwapchainKHR()

- VulkanDevice       
  - vkDestroyCommandPool()
  - vkDestroyDevice();
  - DestroyDebugUtilsMessengerEXT();
  - vkDestroySurfaceKHR();
  - vkDestroyInstance();

- Window
  - glfwCreateWindow()


