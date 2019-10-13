#include "Window.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include <InputManager.h>


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
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    InputManager::Instance()->Checker(action, key, mods);
}

void MyTestCallback(InputBinding* binding)
{
    printf("%s pressed\n", binding->name.c_str());

    if (InputManager::Instance()->UnregisterBinding("[Test] Spacebar", GLFW_KEY_SPACE))
    {
        printf("Successfully unregistered %s\n", binding->name.c_str());
    }
    else
    {
        printf("Failed to unregistered %s\n", binding->name.c_str());
    }
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


    InputManager::Instance()->RegisterBinding("[Test] Spacebar", GLFW_KEY_SPACE, 1, INPUTBINDING_MOD_NONE, MyTestCallback);
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