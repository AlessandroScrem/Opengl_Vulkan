#include "VulkanEngine.hpp"

//std
#include <iostream>
#include <vector>



VulkanEngine::~VulkanEngine() 
{
    std::cout << "VulkanEngine  destructor\n";
}

void VulkanEngine::mainLoop() 
{
    while(!window.shouldClose() ) {
        glfwPollEvents();
    }   
}

void VulkanEngine::run() 
{   
    mainLoop();

}






