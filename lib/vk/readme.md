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

- VulkanCommanBuffer
  - createCommandPool()

- VulkanEngine()
  - createSemaphores()

 ## Destruction order:

- VulkanEngine
  - vkDestroySemaphores()
  
- VulkanCommanBuffer
  - vkDestroyCommandPool()

- VulkanVertexBuffer
  - vkDestroyBuffer();

- VulakanPipeline
  - vkDestroyPipeline(graphicsPipeline)
  - vkDestroyPipelineLayout(pipeLineLayout)

- VulkanSwapchian
  - vkDestroyFramebuffer()
  - vkDestroyImageView()
  - vkDestroySwapchainKHR()
  - vkDestroyRenderPass()

- VulkanDevice       
  - vkDestroyDevice();
  - DestroyDebugUtilsMessengerEXT();
  - vkDestroySurfaceKHR();
  - vkDestroyInstance();

- Window
  - glfwCreateWindow()


