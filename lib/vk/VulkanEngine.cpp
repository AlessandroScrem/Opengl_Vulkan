#include "VulkanEngine.hpp"

//std
#include <iostream>
#include <vector>

VulkanEngine::VulkanEngine()
{    
    std::cout << "VulkanEngine  constructor\n";
}

VulkanEngine::~VulkanEngine() 
{
    std::cout << "VulkanEngine  destructor\n";
}

void VulkanEngine::run() 
{
    while(!window.shouldClose() ) {
        glfwPollEvents();
        drawFrame();
    }   
}


void VulkanEngine::drawFrame() 
{   

}






