#include <NovusTypes.h>
#include <robin_hood.h>
#include "InputBinding.h"

class InputManager
{
public:
    static InputManager* Instance()
    {
        if (!_inputManager)
            _inputManager = new InputManager();

        return _inputManager;
    }

    void Setup();
    bool Checker(i32 action, i32 key, i32 modifiers);
    bool RegisterBinding(std::string bindingName, i32 key, i32 actionMask, i32 modifierMask, InputBindingFunc callback);
    bool UnregisterBinding(std::string bindingName, i32 key);

private:
    robin_hood::unordered_map<i32, std::vector<InputBinding>> _inputBindingMap;
    static InputManager* _inputManager;
};