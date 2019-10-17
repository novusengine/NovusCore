#include "InputManager.h"

void InputManager::Setup()
{
    _inputBindingMap.clear();

    _inputBindingMap[GLFW_KEY_SPACE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_SPACE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_APOSTROPHE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_COMMA] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_MINUS] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_PERIOD] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_SLASH] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_0] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_1] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_2] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_3] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_4] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_5] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_6] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_7] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_8] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_9] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_SEMICOLON] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_EQUAL] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_A] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_B] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_C] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_D] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_E] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_G] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_H] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_I] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_J] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_K] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_L] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_M] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_N] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_O] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_P] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_Q] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_R] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_S] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_T] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_U] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_V] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_W] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_X] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_Y] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_Z] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_LEFT_BRACKET] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_BACKSLASH] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_RIGHT_BRACKET] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_GRAVE_ACCENT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_WORLD_1] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_WORLD_2] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_ESCAPE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_ENTER] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_TAB] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_BACKSPACE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_INSERT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_DELETE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_RIGHT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_LEFT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_DOWN] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_UP] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_PAGE_UP] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_PAGE_DOWN] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_HOME] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_END] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_CAPS_LOCK] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_SCROLL_LOCK] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_NUM_LOCK] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_PRINT_SCREEN] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_PAUSE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F1] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F2] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F3] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F4] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F5] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F6] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F7] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F8] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F9] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F10] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F11] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F12] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F13] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F14] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F15] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F16] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F17] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F18] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F19] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F20] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F21] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F22] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F23] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F24] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_F25] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_0] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_1] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_2] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_3] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_4] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_5] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_6] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_7] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_8] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_9] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_DECIMAL] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_DIVIDE] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_MULTIPLY] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_SUBTRACT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_ADD] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_ENTER] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_KP_EQUAL] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_LEFT_SHIFT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_LEFT_CONTROL] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_LEFT_ALT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_LEFT_SUPER] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_RIGHT_SHIFT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_RIGHT_CONTROL] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_RIGHT_ALT] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_RIGHT_SUPER] = std::vector<InputBinding>();
    _inputBindingMap[GLFW_KEY_MENU] = std::vector<InputBinding>();

    // Micro Optimization (Pre Allocate Memory for 8 InputBindings)
    for (auto inputBinding : _inputBindingMap)
    {
        inputBinding.second.reserve(8);
    }
}

void InputManager::KeyboardInputChecker(Window* window, i32 key, i32 scancode, i32 action, i32 modifiers)
{
    for (auto inputBinding : _inputBindingMap[key])
    {
        if ((inputBinding.actionMask == action || (inputBinding.actionMask & action)) && inputBinding.modifierMask == modifiers)
        {
            if (!inputBinding.callback)
                continue;

            inputBinding.callback(window, &inputBinding);
        }
    }
}

bool InputManager::RegisterBinding(std::string bindingName, i32 key, i32 actionMask, i32 modifierMask, InputBindingFunc callback)
{
    for (auto inputBinding : _inputBindingMap[key])
    {
        if (inputBinding.name == bindingName)
            return false;
    }

    InputBinding newBind(bindingName, actionMask, key, modifierMask, callback);
    _inputBindingMap[key].push_back(newBind);

    return true;
}

bool InputManager::UnregisterBinding(std::string bindingName, i32 key)
{
    bool found = false;
    _inputBindingMap[key].erase(std::remove_if(_inputBindingMap[key].begin(), _inputBindingMap[key].end(), [bindingName, &found](InputBinding binding) { if (binding.name == bindingName) { found = true; return true; } return false; }), _inputBindingMap[key].end());
    return found;
}
