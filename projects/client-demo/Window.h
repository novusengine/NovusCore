#pragma once
#include <NovusTypes.h>

struct GLFWwindow;

class Window
{
public:
    Window();
    ~Window();

    bool Init(u32 width, u32 height);

    bool Update(f32 deltaTime);
    void Render();

private:

private:
    GLFWwindow* _window;

    static bool _glfwInitialized;
};