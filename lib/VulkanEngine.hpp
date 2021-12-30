#pragma once
#include "Engine.hpp"
#include "Window.hpp"

#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
VK_DEFINE_HANDLE(VkInstance)

class VulkanEngine : public Engine
{
public:
    VulkanEngine(){    std::cout << "VulkanEngine  constructor\n";}
    ~VulkanEngine();

    void run();
private:
    void initVulkan();
    void createInstance();
    void setupDebugMessenger();
    void mainLoop();
    void cleanup();

    Window window{EngineType::Vulkan};
    VkInstance instance;
};


