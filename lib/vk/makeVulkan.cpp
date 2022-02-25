#include "../Engine.hpp"
#include "VulkanEngine.hpp"

std::unique_ptr<Engine> Engine::makeVulkan() {
    return std::make_unique<VulkanEngine>();;
}
