#pragma once

#include "input_manager.hpp"

// std
#include <memory>

namespace ngn
{
    class ServiceLocator
    {
    public:
        static inline InputManager* GetInputManager() { return inputManager_.get(); }
        
        static inline void ShutdownServices() { shutdownInputManager(); }

        static inline void Provide() {
            if (inputManager_) return;           
            inputManager_ = std::make_unique<ngn::InputManager>();
        }

    private:
        static inline std::unique_ptr<InputManager> inputManager_ = nullptr;

        static inline void shutdownInputManager() {
            if (!inputManager_) return;
            inputManager_.reset();
        }
    };

} // namespace ngn
