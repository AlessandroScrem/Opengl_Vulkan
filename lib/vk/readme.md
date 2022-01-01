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

- VulkanPipeline

 ## Destruction order:

- VulakanPipeline
  - vkDestroyPipelineLayout()

- VulkanSwapchian
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


