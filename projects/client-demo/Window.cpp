#include "Window.h"
#include <GLFW/glfw3.h>
#include <cassert>


bool Window::_glfwInitialized = false;

Window::Window()
    : _window(nullptr)
{
    
}

Window::~Window()
{
    if (_window != nullptr)
    {
        glfwDestroyWindow(_window);
    }

    if (_inputManager != nullptr)
    {
        delete _inputManager;
    }
}

void error_callback(i32 error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 modifiers)
{
    Window* userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    userWindow->GetInputManager()->KeyboardInputChecker(userWindow, key, scancode, action, modifiers);
}

void CloseWindowInput(Window* window, InputBinding* binding)
{
    glfwSetWindowShouldClose(window->GetWindow(), GLFW_TRUE);
}

bool Window::Init(u32 width, u32 height)
{
    if (!_glfwInitialized)
    {
        if (!glfwInit())
        {
            assert(false);
            return false;
        }
        glfwSetErrorCallback(error_callback);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(width, height, "CNovusCore", NULL, NULL);
    if (!_window)
    {
        assert(false);
        return false;
    }
    glfwSetWindowUserPointer(_window, this);

    _inputManager = new InputManager();
    _inputManager->RegisterBinding("[Test] Close Window", GLFW_KEY_ESCAPE, GLFW_PRESS, INPUTBINDING_MOD_NONE, CloseWindowInput);
    glfwSetKeyCallback(_window, key_callback);

    return true;
}

bool Window::Update(f32 deltaTime)
{
    glfwPollEvents();

    if (glfwWindowShouldClose(_window))
    {
        return false;
    }
    return true;
}

void Window::Present()
{
    //glfwSwapBuffers(_window);
}