#pragma once
#include <NovusTypes.h>

class InputManager;

class Camera
{
public:
    Camera(const Vector3& pos);
    
    void Update(f32 deltaTime);

    Matrix& GetViewMatrix() { return _viewMatrix; }

    void SetInputManager(InputManager* inputManager) { _inputManager = inputManager; }
private:

private:
    Matrix _viewMatrix;
    f32 _movementSpeed = 3.0f;
    f32 _rotationSpeed = 90.0f;
    InputManager* _inputManager;
    f32 _lastDeltaTime = 0.0f;
};