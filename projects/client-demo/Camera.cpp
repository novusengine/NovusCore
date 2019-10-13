#include "Camera.h"
#include <windows.h>

Camera::Camera(const Vector3& pos)
    : _viewMatrix()
{
    _viewMatrix.pos = pos;
}


void Camera::Update(f32 deltaTime)
{
    // Movement
    if (GetAsyncKeyState('W'))
    {
        _viewMatrix.pos += _viewMatrix.at * _movementSpeed * deltaTime;
    }
    if (GetAsyncKeyState('S'))
    {
        _viewMatrix.pos += _viewMatrix.at * -_movementSpeed * deltaTime;
    }
    if (GetAsyncKeyState('A'))
    {
        _viewMatrix.pos += _viewMatrix.right * -_movementSpeed * deltaTime;
    }
    if (GetAsyncKeyState('D'))
    {
        _viewMatrix.pos += _viewMatrix.right * _movementSpeed * deltaTime;
    }
    if (GetAsyncKeyState(VK_SPACE))
    {
        _viewMatrix.pos += _viewMatrix.up * _movementSpeed * deltaTime;
    }
    if (GetAsyncKeyState(VK_SHIFT))
    {
        _viewMatrix.pos += _viewMatrix.up * -_movementSpeed * deltaTime;
    }

    // Rotation
    if (GetAsyncKeyState(VK_UP))
    {
        _viewMatrix.RotateX(-_rotationSpeed * deltaTime, Matrix::MultiplicationType::PRE);
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        _viewMatrix.RotateX(_rotationSpeed * deltaTime, Matrix::MultiplicationType::PRE);
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        _viewMatrix.RotateY(_rotationSpeed * deltaTime, Matrix::MultiplicationType::PRE);
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        _viewMatrix.RotateY(-_rotationSpeed * deltaTime, Matrix::MultiplicationType::PRE);
    }
}