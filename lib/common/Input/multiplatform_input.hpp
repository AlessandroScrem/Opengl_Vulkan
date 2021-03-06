#pragma once

#include "input_key.hpp"
#include "input_devices.hpp"
// std
#include <unordered_map>

namespace ngn {

    struct Cursorpos{
        float x{0.f};
        float y{0.f};
    };
    struct Winstat{
        int w = 0;
        int h = 0;
        bool resized  = false;
        bool iconized = false;
    };

    class MultiplatformInput
    {       
    public:
        std::unordered_map<InputKey, InputDeviceState> GetKeyboardState(int index) { return keyboardState_; }
        std::unordered_map<InputKey, InputDeviceState> GetMouseState(int index) { return mouseState_; }

        void UpdateKeyboardState(int key, float value);
        void UpdateMouseState(int button, float value);
        void UpdateMousePos(float posx, float posy);
        void UpdateWindowSize(int w, int h);
        void Resized(bool stat);
        void Iconized(bool stat);
        std::pair<float, float> GetMousePosition(){return std::make_pair(pos_.x, pos_.y);}

        Winstat winstat_{};
        Cursorpos pos_{};
    private:
        static InputKey multiplatformKeyToInputKey(int key);
        static InputKey multiplatformMouseButtonToInputKey(int button);
    private:
        std::unordered_map<InputKey, InputDeviceState> keyboardState_ {};
        std::unordered_map<InputKey, InputDeviceState> mouseState_ {};
    };
} // namespace ngn

