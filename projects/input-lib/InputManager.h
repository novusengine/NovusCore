#include <NovusTypes.h>
#include <robin_hood.h>
#include "InputBinding.h"

class InputManager
{
public:
    void Setup();
    void KeyboardInputChecker(Window* window, i32 key, i32 scancode, i32 action, i32 modifiers);
    bool RegisterBinding(std::string bindingName, i32 key, i32 actionMask, i32 modifierMask, InputBindingFunc callback);
    bool UnregisterBinding(std::string bindingName, i32 key);

private:
    robin_hood::unordered_map<i32, std::vector<InputBinding>> _inputBindingMap;
};