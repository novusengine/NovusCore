#include <NovusTypes.h>
#include <GLFW/glfw3.h>

enum InputBindingModifier
{
    INPUTBINDING_MOD_NONE = 0x00,
    INPUTBINDING_MOD_SHIFT = 0x01,
    INPUTBINDING_MOD_CONTROL = 0x02,
    INPUTBINDING_MOD_ALT = 0x04
};

class Window;
class InputBinding;
typedef void InputBindingFunc(Window*, InputBinding*);
class InputBinding
{
public:
    InputBinding() : name("invalid"), actionMask(0), key(-1), modifierMask(0), callback(nullptr) { }
    InputBinding(std::string inName, i32 inActionMask, i32 inKey, i32 inModifierMask, InputBindingFunc inCallback) : name(inName), actionMask(inActionMask), key(inKey), modifierMask(inModifierMask), callback(inCallback) { }

public:
    std::string name;
    i32 actionMask;
    i32 key;
    i32 modifierMask;
    InputBindingFunc* callback;
};