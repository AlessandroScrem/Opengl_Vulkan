#include "input_manager.hpp"
#include "mytypes.hpp"
// std

namespace ngn
{
       InputSource GetInputSourceFromKey(InputKey key) {
        switch (key) {
        case InputKey::KEY_A:
        case InputKey::KEY_B:
        case InputKey::KEY_C:
        case InputKey::KEY_D:
        case InputKey::KEY_E:
        case InputKey::KEY_S:
        case InputKey::KEY_O:
        case InputKey::KEY_L:
        case InputKey::KEY_UP:
        case InputKey::KEY_DOWN:
        case InputKey::KEY_X:
        case InputKey::KEY_Z:
            return InputSource::KEYBOARD;
        case InputKey::GAMEPAD_L_THUMB_X:
        case InputKey::GAMEPAD_L_THUMB_Y:
        case InputKey::GAMEPAD_R_THUMB_X:
        case InputKey::GAMEPAD_R_THUMB_Y:
        case InputKey::GAMEPAD_R_TRIGGER:
        case InputKey::GAMEPAD_L_TRIGGER:
        case InputKey::GAMEPAD_Y:
        case InputKey::GAMEPAD_X:
        case InputKey::GAMEPAD_B:
        case InputKey::GAMEPAD_A:
        case InputKey::GAMEPAD_START:
        case InputKey::GAMEPAD_SELECT:
        case InputKey::GAMEPAD_BUMPER_R:
        case InputKey::GAMEPAD_BUMPER_L:
        case InputKey::GAMEPAD_L3:
        case InputKey::GAMEPAD_R3:
        case InputKey::GAMEPAD_DPAD_UP:
        case InputKey::GAMEPAD_DPAD_RIGHT:
        case InputKey::GAMEPAD_DPAD_LEFT:
        case InputKey::GAMEPAD_DPAD_DOWN:
            return InputSource::GAMEPAD;
        case InputKey::MOUSE_LEFT:
        case InputKey::MOUSE_RIGHT:
        case InputKey::MOUSE_MIDDLE:
        case InputKey::MOUSE_MOVE_X:
        case InputKey::MOUSE_MOVE_Y:
            return InputSource::MOUSE;
        default:
            return InputSource::UNKNOWN;
        }
    }


    InputManager::InputManager() {
        SPDLOG_TRACE("Input Manager intialized!");
    }

    InputManager::~InputManager() {
    }

    void InputManager::RegisterDevice(const InputDevice& device) {
        spdlog::info("Device registered of type: {}", static_cast<int>(device.Type) );
        devices_.emplace_back(device);
        spdlog::info("Device #: {}" ,devices_.size() );
    }
    
    void InputManager::RemoveDevice(InputDeviceType source, int inputIndex) 
    {
        
    }
    
    void InputManager::RegisterActionCallback(const std::string &actionName, const ActionCallback& callback) 
    {
        actionCallbacks_[actionName].emplace_back(callback);      
    }
    
    void InputManager::RemoveActionCallback(const std::string& actionName, const std::string& callbackRef) 
    {
        erase_if(actionCallbacks_[actionName], [callbackRef](const ActionCallback& callback) {
            return callback.Ref == callbackRef;
        });   
    }
    
    void InputManager::MapInputToAction(InputKey key, const InputAction& action) 
    {
        // TODO: Check for duplicates
        inputActionMapping_[key].emplace_back(action);    
    }
    
    void InputManager::UnmapInputFromAction(InputKey key, const std::string& action) 
    {
         erase_if(inputActionMapping_[key], [action](const InputAction& inputAction) {
            return inputAction.actionName == action;
        });       
    }
    
    std::vector<InputManager::ActionEvent> InputManager::generateActionEvent(int DeviceIndex, InputKey key, float newVal) {
        auto& actions = inputActionMapping_[key];

        std::vector<ActionEvent> actionEvents {};

        InputSource source = GetInputSourceFromKey(key);

        for (auto& action : actions) {
            actionEvents.emplace_back(ActionEvent {
                .ActionName = action.actionName,
                .Source = source,
                .SourceIndex = DeviceIndex,
                .Value = newVal * action.scale
            });
        }

        return actionEvents;
    }
    
    void InputManager::propagateActionEvent(ActionEvent event) 
    {
        for (size_t i = actionCallbacks_[event.ActionName].size() - 1; i >= 0; i--) {
            auto& actionCallback = actionCallbacks_[event.ActionName][i];

            if (actionCallback.Func(event.Source, event.SourceIndex, event.Value)) break;
        }        
    }

    void InputManager::processInput()
    {
         std::vector<ActionEvent> events {};
        for (auto& device : devices_) {
            // get new state for device
            auto newState = device.StateFunc(device.Index);

            // compare to old state for device
            for (auto& keyState : newState) {
                if (device.CurrentState[keyState.first].value != keyState.second.value) {
                    //TODO: Fix cases where conflicting mappings -- additive fashion?
                    auto generatedEvents = generateActionEvent(device.Index, keyState.first,keyState.second.value);
                    events.insert(events.end(), generatedEvents.begin(), generatedEvents.end());
                    // save new state value
                    device.CurrentState[keyState.first].value = keyState.second.value;
                }
            }
        }

        // propagate action events
        for (auto& event : events) {
            propagateActionEvent(event);
        }       
    }

} // namespace ngn
