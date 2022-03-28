#pragma once

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanUbo.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanShader.hpp"
#include "../Engine.hpp"
//common lib
#include <Window.hpp>
#include <multiplatform_input.hpp>
#include <unordered_map>
#include <deque>

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call functors
        }

        deletors.clear();
    }
};

struct FrameData {
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;

	DeletionQueue _frameDeletionQueue;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	AllocatedBuffer cameraBuffer;
	VkDescriptorSet globalDescriptor;

	AllocatedBuffer objectBuffer;
	VkDescriptorSet objectDescriptor;
};

struct Material{
    VulkanShader *shader;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
};

struct RenderObject{
    VulkanVertexBuffer *vertexBuffer;
    Material *material;
};

const unsigned int MAX_FRAMES_IN_FLIGHT = 2;    

class VulkanEngine : public Engine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    void run() override;
    void setWindowMessage(std::string msg) override{window.setWindowMessage(msg);};


private:
    // void upload_models();
    // void create_defaultPipeline();

    // -----------------------
    // -----------------------
    void drawFrame();

    void init_commands();
    void init_sync_structures();

    void draw();
    void draw_objects(VkCommandBuffer cmd,RenderObject* first, int count);  
    // -----------------------
    // -----------------------
    
    void updateUbo();

    void createSyncObjects();
    void recreateSwapChain();
    void createCommandBuffers();
    void cleanupCommandBuffers();

    Material* create_material(VkPipeline pipeline, VkPipelineLayout layout, ShaderType type, std::string& name);
    Material* get_material(const std::string& name);
    VulkanVertexBuffer* get_vertexBuffer(const std::string& name);

    std::vector<RenderObject> _renderables;
    std::unordered_map<std::string, Material> _materials;
    std::unordered_map<std::string, VulkanVertexBuffer*> _vertexbuffers;

    Window window{EngineType::Vulkan, Engine::input_};
    
    VulkanDevice device{window};
    VulkanSwapchain swapchain{device, window};
    VulkanUbo ubo{device, swapchain};
    VulkanImage vulkanimage{device, swapchain};
    VulkanVertexBuffer vertexbuffer{device, swapchain, ubo, vulkanimage, Engine::model};
    VulkanShader vulkanshader{device, Engine::glslShader};
    VulkanPipeline pipeline{device, swapchain, vertexbuffer, vulkanshader};

    std::vector<VkCommandBuffer> commandBuffers;

    //  GPU-GPU synchronization
    // std::vector<VkSemaphore> imageAvailableSemaphores;
    // std::vector<VkSemaphore> renderFinishedSemaphores;

    // //  CPU-GPU synchronization
    // std::vector<VkFence> inFlightFences;
    // std::vector<VkFence> imagesInFlight;

    // size_t currentFrame = 0;


    //------------------------------------
    //------------------------------------
    
    VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;
    DeletionQueue _mainDeletionQueue;

    VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

    int _frameNumber {0};

    //------------------------------------
    //------------------------------------

};


