#include "Engine.hpp"
// common lib
#include <service_locator.hpp>
//libs
#include <spdlog/spdlog.h>

Engine::Engine(){
    SPDLOG_TRACE("constructor");

    // provide input manager
    ngn::ServiceLocator::Provide(new ngn::InputManager());
    MapActions();       
}

std::unique_ptr<Engine>
Engine::create(EngineType type)
{
    std::unique_ptr<Engine> window;

    if(type == EngineType::Opengl){
        //window = std::make_unique<OpenGLEngine>();
        window = makeOpengl();
    }

    if(type == EngineType::Vulkan){
        //window = std::make_unique<VulkanEngine>();
        window = makeVulkan();
    }

    return window;   
}

void Engine::updateEvents() 
{
    if (ngn::ServiceLocator::GetInputManager()) {
        ngn::ServiceLocator::GetInputManager()->processInput();
    }

    std::string cmd;
    for(auto const& command : commands_){
        command.second->Execute();
        cmd = command.first;
    }
    std::stringstream msg;
    msg << " [ " <<  ngn::Time::getFps() << " ms/frame ]" << " Focal = " << ourCamera.GetFocal() ;
    msg << " Cmd  " << cmd ;
    msg << " " << ngn::Mouse::getDirection_str();
    msg << " " << glm::to_string(ngn::Mouse::getDirection());
    setWindowMessage(msg.str());
}


void Engine::MapActions() 
{
    SPDLOG_TRACE("MapActions");

    using namespace ngn;

    auto* inputManager_ = ServiceLocator::GetInputManager();

    if(!inputManager_){
        spdlog::error("failed to get inputManager pointer");
        return;
    }

    // Register input devices
    inputManager_->RegisterDevice(InputDevice {
        .Type = InputDeviceType::KEYBOARD,
        .Index = 0,
        .StateFunc = std::bind(&MultiplatformInput::GetKeyboardState, &input_, std::placeholders::_1)
    });

    inputManager_->RegisterDevice(InputDevice {
        .Type = InputDeviceType::MOUSE,
        .Index = 0,
        .StateFunc = std::bind(&MultiplatformInput::GetMouseState, &input_, std::placeholders::_1)
    });


    // Map inputs
    inputManager_->MapInputToAction(InputKey::KEY_A, InputAction {
        .actionName = "orbit left/right",
        .scale = -1.f
    });
    inputManager_->MapInputToAction(InputKey::KEY_D, InputAction {
        .actionName = "orbit left/right",
        .scale = 1.f
    });
    inputManager_->MapInputToAction(InputKey::KEY_W, InputAction {
        .actionName = "orbit up/down",
        .scale = -1.f
    });    
    inputManager_->MapInputToAction(InputKey::KEY_S, InputAction {
        .actionName = "orbit up/down",
        .scale = 1.f
    });
    inputManager_->MapInputToAction(InputKey::KEY_UP, InputAction {
        .actionName = "cam fov",
        .scale = -1.f
    });
    inputManager_->MapInputToAction(InputKey::KEY_DOWN, InputAction {
        .actionName = "cam fov",
        .scale = 1.f
    });
    inputManager_->MapInputToAction(InputKey::MOUSE_LEFT, InputAction {
        .actionName = "leftclick",
        .scale = 1.f
    });
    inputManager_->MapInputToAction(InputKey::MOUSE_MIDDLE, InputAction {
        .actionName = "middleclick",
        .scale = 1.f
    });
    inputManager_->MapInputToAction(InputKey::MOUSE_RIGHT, InputAction {
        .actionName = "rightclick",
        .scale = 1.f
    });

    inputManager_->RegisterActionCallback("orbit left/right", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {

            if (value){
                // step = rad / sec
                value /= 180;
                commands_.emplace("orbit left/right", std::make_unique<CmdOrbit>(ourCamera, glm::vec2(value, 0.f)) );  
                shouldupdate = true;
                
            }else{
                commands_.erase("orbit left/right");                       
                shouldupdate = false;
            }
            return true;
        }
    });

    inputManager_->RegisterActionCallback("orbit up/down", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {

            if (value){
                // step = rad / sec
                value /= 180;
                commands_.emplace("orbit up/down", std::make_unique<CmdOrbit>(ourCamera, glm::vec2(0.f, value)) );  
                shouldupdate = true;
            }else{
                commands_.erase("orbit up/down"); 
                shouldupdate = false;
            }
            return true;
        }
    });

    inputManager_->RegisterActionCallback("cam fov", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {

            if (value){
                // step = 10 / sec
                float step = 2.0f * Time::getFrameTime();
                value *= step;
                commands_.emplace("cam fov", std::make_unique<CmdFov>(ourCamera, glm::vec2(value)) ); 
                shouldupdate = true;
            }else{
                commands_.erase("cam fov"); 
                shouldupdate = false;
            }
            return true;
        }
    });

    inputManager_->RegisterActionCallback("leftclick", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {
            if(value) // btn down
            {
                Mouse::Start();
                commands_.emplace("click orbit", std::make_unique<CmdOrbit>(ourCamera, glm::vec2(0.f)) );  
                shouldupdate = true;
            }else // btn up
            {
                Mouse::Stop();
                commands_.erase("click orbit");
                shouldupdate = false;
            }
            return true;
        }
    });
    inputManager_->RegisterActionCallback("middleclick", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {
            if(value) // btn down
            {
                Mouse::Start();
                commands_.emplace("click pan", std::make_unique<CmdPan>(ourCamera, glm::vec2(0.f)) ); 
                shouldupdate = true;
            }else // btn up
            {
                Mouse::Stop();
                commands_.erase("click pan");
                shouldupdate = false;
            }
            return true;
        }
    });
    inputManager_->RegisterActionCallback("rightclick", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {
            if(value) // btn down
            {
                Mouse::Start();
                commands_.emplace("click roll", std::make_unique<CmdRoll>(ourCamera, glm::vec2(0.f)) ); 
                shouldupdate = true;
            }else // btn up
            {
                Mouse::Stop();
                    commands_.erase("click roll");
                shouldupdate = false;
            }
            return true;
        }
    });
    
}


