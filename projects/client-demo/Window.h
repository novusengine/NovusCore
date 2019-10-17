#pragma once
#include <NovusTypes.h>
#include <InputManager.h>

struct GLFWwindow;
class Window
{
public:
    Window();
    ~Window();

    bool Init(u32 width, u32 height);

    bool Update(f32 deltaTime);
    void Present();

    GLFWwindow* GetWindow() { return _window; }
    InputManager* GetInputManager() { return _inputManager; }

private:

private:
    GLFWwindow* _window;
    InputManager* _inputManager;

    static bool _glfwInitialized;
};