#include "VulkanEngine.hpp"

std::unique_ptr<Engine> Engine::makeVulkan(EngineType type) {
    return std::make_unique<VulkanEngine>(type);;
}
