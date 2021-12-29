#pragma once
#include "Window.hpp"

struct GLFWwindow;
#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
VK_DEFINE_HANDLE(VkInstance)

class VulkanWindow : public Window
{
public:
    VulkanWindow(){    std::cout << "VulkanWindow  constructor\n";}
    ~VulkanWindow();

    void run();
private:
    void initWindow();
    void initVulkan();
    void createInstance();
    void mainLoop();
    void cleanup();

    GLFWwindow* window = nullptr;
    VkInstance instance;   
};


