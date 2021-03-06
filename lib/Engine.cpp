#include "Engine.hpp"
#include "GUI.h"
// common lib
#include <Window.hpp>
#include <service_locator.hpp>
#include <model.hpp>
//lib
// #include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
// std
#include <memory>

Engine::Engine(EngineType type) : engine_type_{type}
{
    SPDLOG_DEBUG("constructor");

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

void Engine::run() 
{ 
    auto timestamp = [=]() {      
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


        ngn::Time::start();
            draw();
        ngn::Time::end();

        updateEvents();
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

    window_->update();
    if (window_->is_Resized()){
        resizeFrame(); 
    }

}

UniformBufferObject Engine::getMVP()
{   
    UniformBufferObject mvp{};
    const float NEAR = 1.0;
    const float FAR = 11.0;

    mvp.view = ourCamera.GetViewMatrix();
    mvp.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window_->getWindowAspect(), NEAR, FAR);

    mvp.viewPos = ourCamera.GetPosition();

    return mvp;
} 


void Engine::draw_UiOverlay()
{
    static size_t selected = 0;
 
    std::vector<std::string> items{};
    for( auto & obj : renderables_){
        items.push_back(obj->objName);
    }
    
    Transformations t = renderables_.at(selected)->objNode.get();
 
    if(GUI::ObjectNode(t, items, selected)){
        renderables_.at(selected)->objNode.set(t);
    }  
}

void Engine::init_shaders()
{
    SPDLOG_TRACE("Engine::init_shaders");

    {
        auto shader = Shader::make().type(GLSL::TEXTURE)
                                    .addTexture("data/textures/viking_room.png", 1)
                                    .build(); 

        shaders_.emplace("texture", std::move(shader));                                 
    }
    {
        auto shader = Shader::make().type(GLSL::NORMALMAP)
                                    .build();

        shaders_.emplace("normalmap", std::move(shader));
    }
    {
        auto shader = Shader::make().type(GLSL::PHONG)
                                    .build();

        shaders_.emplace("phong", std::move(shader));
    }
}

void Engine::init_fixed_shaders()
{
   {
        auto shader = Shader::make().type(GLSL::AXIS)
                                    .setPolygonMode(GLSL::LINES)
                                    .build();

        fixed_shaders_.emplace("axis", std::move(shader));
    }
}

void Engine::init_fixed()
{
   SPDLOG_TRACE("Engine init_fixed"); 

    {
        auto object = RenderObject::make().build(Model::axis(), "axis");
        fixed_objects_.emplace("axis", std::move(object)); 
    }        
}

void Engine::init_renderables()
{
   SPDLOG_TRACE("Engine init_renderables"); 

   {
        Model model("data/models/sphere/sphere_scaled.obj", Model::UP::ZUP);
        // Transformations tra{};
        // tra.S ={1.0f, 1.0f, 1.0f};
        // //rotate toward camera
        // tra.R ={0.0f, 270.0f, 0.0f};
        // // move right
        // tra.T = {1.0f, 0.0f, 0.0f};
        // model.node.set(tra);

        auto object = RenderObject::make().build(model, "phong");
        object->objName = "sphere";
        renderables_.push_back(std::move(object));
    }
   {
        Model model("data/models/viking_room.obj", Model::UP::ZUP);
        // rotate toward camera
        Transformations tra{};
        tra.R ={0.0f, 270.0f, 0.0f};
        // move right
        tra.T = {1.0f, 0.0f, 0.0f};
        model.node.set(tra);

        auto object = RenderObject::make().build(model, "texture");
        object->objName = "viking_room";
        renderables_.push_back(std::move(object));
    }

    {
        Model model("data/models/suzanne.obj", Model::UP::YUP);
        Transformations tra{};
        // move left
        tra.T = {-1.0f, 0.0f, 0.0f};
        model.node.set(tra);

        auto object = RenderObject::make().build(model, "normalmap");
        object->objName = "suzanne";
        renderables_.push_back(std::move(object));

    } 
}


void Engine::MapActions() 
{
    SPDLOG_TRACE("MapActions");

    using namespace ngn;

    const float MULT = 0.01f;
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
                float step = MULT * ngn::Time::getFrameTime();
                value *= step;
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
                float step = MULT * ngn::Time::getFrameTime();
                value *= step;
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
                float step = MULT * 10 * Time::getFrameTime();
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
                float step = MULT * Time::getFrameTime();
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


