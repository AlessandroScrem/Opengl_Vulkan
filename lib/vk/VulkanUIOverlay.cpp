#include "VulkanUIOverlay.h"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

void VulkanUIOverlay::init()
{

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(windowPtr, true);

    VkDevice                 g_Device           = device->getDevice();
    VkPhysicalDevice         g_PhysicalDevice   = device->getPhysicalDevice();
    VkInstance               g_Instance         = device->getInstance();
    uint32_t                 g_QueueFamily      = device->getQueueFamiliesIndices().graphicsFamily.value(); //only grahics family;
    VkQueue                  g_Queue            = device->getGraphicsQueue();                                 //only grahics Queue
    int                      g_MinImageCount    = device->getSwapChainSupport().capabilities.minImageCount;
    int                      g_ImageCount       = g_MinImageCount + 1;
    VkRenderPass             g_RenderPass       = swapchain->getRenderpass();
    VkSampleCountFlagBits    g_MSAASamples      = device->getMsaaSamples();
    auto                     g_CheckVkResultFn  = [](VkResult x){ VK_CHECK_RESULT(x);};            
    VkAllocationCallbacks*   g_Allocator        = NULL;
    VkPipelineCache          g_PipelineCache    = VK_NULL_HANDLE;

    // Create Descriptor Pool
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        VK_CHECK_RESULT(vkCreateDescriptorPool(g_Device, &pool_info, NULL, &_gui_DescriptorPool));
    }

    // init ImGui_ImplVulkan
    {
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = g_Instance;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily =  g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = _gui_DescriptorPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = g_ImageCount;
        init_info.MSAASamples = g_MSAASamples;
        init_info.Allocator = g_Allocator;
        init_info.CheckVkResultFn = g_CheckVkResultFn;

        ImGui_ImplVulkan_Init(&init_info, g_RenderPass);
    }    

    // Upload Fonts
    {
        auto cmd = device->beginSingleTimeCommands();
        
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        
        device->endSingleTimeCommands(cmd);
        
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void VulkanUIOverlay::cleanup()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    vkDestroyDescriptorPool(device->getDevice(), _gui_DescriptorPool, nullptr);  
}

void VulkanUIOverlay::newFrame()
{
    // feed inputs to dear imgui, start new frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void VulkanUIOverlay::draw(VkCommandBuffer cmd)
{
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);
}
