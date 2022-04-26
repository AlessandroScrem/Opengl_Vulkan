
#pragma once

#include "input_key.hpp"

// std
#include <functional>

namespace ngn {
    enum class InputDeviceType {
        KEYBOARD,
        MOUSE
    };

    struct InputDeviceState {
        float value { -99.f };
    };

    using InputDeviceStateCallbackFunc = std::function<std::unordered_map<InputKey, InputDeviceState>(int)>;

    struct InputDevice {
        InputDeviceType Type;
        int Index;
        std::unordered_map<InputKey, InputDeviceState> CurrentState;
        InputDeviceStateCallbackFunc StateFunc;
    };
} // namespace ngn