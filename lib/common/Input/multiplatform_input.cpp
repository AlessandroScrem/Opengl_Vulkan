#include "multiplatform_input.hpp"
// lib
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace ngn
{
  
    void MultiplatformInput::UpdateKeyboardState(int key, float value) {
        InputKey iKey = multiplatformKeyToInputKey(key);

        keyboardState_[iKey].value = value;
    }

    void MultiplatformInput::UpdateMouseState(int button, float value) {
        InputKey iKey = multiplatformMouseButtonToInputKey(button);

        mouseState_[iKey].value = value;
    }
    
    void MultiplatformInput::UpdateMousePos(float posx, float posy) 
    {
        pos_.x = posx;
        pos_.y = posy;
    }

    void MultiplatformInput::UpdateWindowSize(int w, int h) 
    { 
        winstat_.w = w; 
        winstat_.h = h; 
    } 

    void MultiplatformInput::Resized(bool stat) 
    { 
        winstat_.resized = stat;
    }

    void MultiplatformInput::Iconized(bool stat) 
    { 
        winstat_.iconized =  stat; 
    }

    InputKey MultiplatformInput::multiplatformKeyToInputKey(int key) {
        switch (key) {
            case GLFW_KEY_A:
                return InputKey::KEY_A;
            case GLFW_KEY_B:
                return InputKey::KEY_B;
            case GLFW_KEY_C:
                return InputKey::KEY_C;
            case GLFW_KEY_D:
                return InputKey::KEY_D;
            case GLFW_KEY_E:
                return InputKey::KEY_E;
            case GLFW_KEY_S:
                return InputKey::KEY_S;
            case GLFW_KEY_O:
                return InputKey::KEY_O;
            case GLFW_KEY_L:
                return InputKey::KEY_L;
            case GLFW_KEY_W:
                return InputKey::KEY_W;
           case GLFW_KEY_X:
                return InputKey::KEY_X;
            case GLFW_KEY_Z:
                return InputKey::KEY_Z;
            case GLFW_KEY_UP:
                return InputKey::KEY_UP;
            case GLFW_KEY_DOWN:
                return InputKey::KEY_DOWN;
            case GLFW_KEY_SPACE:
                return InputKey::KEY_SPACE;

           default:
                return InputKey::UNKNOWN;
        }
    }

    InputKey MultiplatformInput::multiplatformMouseButtonToInputKey(int button) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                return InputKey::MOUSE_LEFT;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return InputKey::MOUSE_RIGHT;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return InputKey::MOUSE_MIDDLE;
            default:
                return InputKey::UNKNOWN;
        }
    }

      
} // namespace ngn




