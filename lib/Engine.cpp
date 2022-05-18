#include "Engine.hpp"
// common lib
#include <Window.hpp>
#include <service_locator.hpp>
// std
#include <memory>

Engine::Engine(EngineType type){
    SPDLOG_DEBUG("constructor");

    engine_type_ = type;

    // provide input manager
    ngn::ServiceLocator::Provide();
    MapActions();

    if(!window_){
        window_ = std::make_unique<Window>();
    }
    window_->init(type);
    window_->registerCallbacks(input_);
}

Engine::~Engine(){
    SPDLOG_DEBUG("destructor");    
}


std::unique_ptr<Engine>
Engine::create(EngineType type)
{
    std::unique_ptr<Engine> engine;

    if(type == EngineType::Opengl){
        engine = makeOpengl(type);
    }

    if(type == EngineType::Vulkan){
        engine = makeVulkan(type);
    }

    return engine;   
}

void Engine::setWindowMessage(std::string msg) 
{
    window_->setWindowMessage(msg);
}

#include <chrono>
void Engine::run() 
{ 

    auto timestamp = [&]() {      
        if(ngn::Time::time())
        {
            std::stringstream msg;
            msg << " [ " <<  ngn::Time::getFrameTime() << " ns/frame ]";
            msg << " [ " <<  ngn::Time::geFps()  << " FPS ]" ;
            msg << " Focal = " << ourCamera.GetFocal() ;
            msg << " " << ngn::Mouse::getDirection_str();
            setWindowMessage(msg.str());        
        } };

    spdlog::info("*******           START           ************");  

    while(!window_->shouldClose() ) {

        window_->update();        
        updateEvents();

        ngn::Time::start();

        draw();
        
        ngn::Time::end();
        timestamp();

    }
    
    spdlog::info("*******           END             ************");  
}

void Engine::updateEvents() 
{
    if (ngn::ServiceLocator::GetInputManager()) {
        ngn::ServiceLocator::GetInputManager()->processInput();
    }

    for(auto const& command : commands_){
        command.second->Execute();
    }
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
    inputManager_->MapInputToAction(InputKey::KEY_O, InputAction {
        .actionName = "dolly in/out",
        .scale = 1.f
    });
    inputManager_->MapInputToAction(InputKey::KEY_L, InputAction {
        .actionName = "dolly in/out",
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
    inputManager_->MapInputToAction(InputKey::KEY_SPACE, InputAction {
        .actionName = "toggle shader",
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
                // speed = 180° / sec
                value *= ( ngn::Time::getFrameTime() / 3.14f  );
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
                // speed = 180° / sec
                value *= ( ngn::Time::getFrameTime() / (3.14f )  );
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

    inputManager_->RegisterActionCallback("dolly in/out", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {

            if (value){
                // step = 10 / sec
                float step = 2.0f * Time::getFrameTime();
                value *= step;
                commands_.emplace("dolly in/out", std::make_unique<CmdDolly>(ourCamera, glm::vec2(value)) ); 
                shouldupdate = true;
            }else{
                commands_.erase("dolly in/out"); 
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
    inputManager_->RegisterActionCallback("toggle shader", InputManager::ActionCallback {
        .Ref = "YoutubeGame",
        .Func = [this](InputSource source, int sourceIndex, float value) {

            if (value){
                model_index_ += 1;
                if(model_index_ >= renderables_.size()) model_index_ = 0;
                shouldupdate = true;               
            }else{                     
                shouldupdate = false;
            }
            return true;
        }
    });
    
    
}


