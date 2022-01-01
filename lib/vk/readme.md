# Vulkan

 ##  Constuction order:


- Window()
  - glfwCreateWindow()

- VulkanDevice()
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


- VulkanSwapchian()
  - vkDestroyImageView()
  - vkDestroySwapchainKHR()

- VulkanDevice()        
  - vkDestroyDevice();
  - DestroyDebugUtilsMessengerEXT();
  - vkDestroySurfaceKHR();
  - vkDestroyInstance();

- Window()
  - glfwCreateWindow()


