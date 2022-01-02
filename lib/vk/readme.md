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

- VulkanCommanBuffer
  - createCommandPool()

 ## Destruction order:

- VulkanCommanBuffer
  - vkDestroyCommandPool()

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


