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

- VulkanPipeline

 ## Destruction order:

- VulakanPipeline
  - vkDestroyPipelineLayout()

- VulkanSwapchian
  - vkDestroyImageView()
  - vkDestroySwapchainKHR()

- VulkanDevice       
  - vkDestroyDevice();
  - DestroyDebugUtilsMessengerEXT();
  - vkDestroySurfaceKHR();
  - vkDestroyInstance();

- Window
  - glfwCreateWindow()


