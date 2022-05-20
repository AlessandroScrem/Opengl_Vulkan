#include "VulkanEngine.hpp"

std::unique_ptr<Engine> Engine::makeVulkan(EngineType type) {
    auto result = std::make_unique<VulkanEngine>(type);
    return result;
}
