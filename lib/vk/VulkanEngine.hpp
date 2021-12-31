#pragma once
#include "Engine.hpp"
#include "common/Window.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

/*
***************************
 vulkan constuction order:
***************************

    Window()
        glfwCreateWindow()

    VulkanDevice()
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();

    VulkanSwapchain
        createSwapChain();

***************************
 vulkan destruction order:
***************************

    VulkanSwapchian()
        vkDestroySwapchainKHR()

    VulkanDevice()        
        vkDestroyDevice(device, nullptr);
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

    Window()
        glfwCreateWindow()

*/

class VulkanEngine : public Engine
{
public:
    VulkanEngine(){    std::cout << "VulkanEngine  constructor\n";}
    ~VulkanEngine();

    void run();
private:
    void mainLoop();

    Window window{EngineType::Vulkan};
    VulkanDevice vkdevice{window};
    VulkanSwapchain swapchain{vkdevice, window};
};


