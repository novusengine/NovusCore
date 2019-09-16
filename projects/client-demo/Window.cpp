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
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

    _window = glfwCreateWindow(width, height, "CNovusCore", NULL, NULL);
    if (!_window)
    {
        assert(false);
        return false;
    }
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

void Window::Render()
{
    glfwSwapBuffers(_window);
}