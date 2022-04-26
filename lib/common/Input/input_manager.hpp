#pragma once

#include "input_devices.hpp"

namespace ngn
{
    class InputManager
    {
    public:
        using ActionCallbackFunc = std::function<bool(InputSource, int, float)>;

        struct ActionCallback {
            std::string Ref;
            ActionCallbackFunc Func;
        };


        InputManager();
        ~InputManager();

        void RegisterDevice(const InputDevice& device);
        void RemoveDevice(InputDeviceType source, int inputIndex);

        void RegisterActionCallback(const std::string &actionName, const ActionCallback& callback);
        void RemoveActionCallback(const std::string& actionName, const std::string& callbackRef);

        void MapInputToAction(InputKey key, const InputAction& action);
        void UnmapInputFromAction(InputKey key, const std::string& action);
        
        void processInput();

    private:
        struct ActionEvent {
            std::string ActionName;
            InputSource Source;
            int SourceIndex;
            float Value;
        };

        std::vector<ActionEvent> generateActionEvent(int deviceIndex, InputKey key, float newVal);
        void propagateActionEvent(ActionEvent event);

        // processInput will get new device state and compare with old state; then generate action events

        std::vector<InputDevice> devices_;
        std::unordered_map<InputKey, std::vector<InputAction>> inputActionMapping_ {};
        std::unordered_map<std::string, std::vector<ActionCallback>> actionCallbacks_ {};
    };
} // namespace ngn
