#include "Vector3.h"
#include "Vector2.h"

// Right handed coordinate system, positive X points north, positive Y points west and positive Z points up
Vector3 Vector3::One = Vector3(1.0f);
Vector3 Vector3::Zero = Vector3(0.0f);
Vector3 Vector3::Up = Vector3(0.0f, 1.0f, 0.0f);
Vector3 Vector3::Down = Vector3(0.0f, -1.0f, 0.0f);
Vector3 Vector3::Left = Vector3(-1.0f, 0.0f, 0.0f);
Vector3 Vector3::Right = Vector3(1.0f, 0.0f, 0.0f);
Vector3 Vector3::Forward = Vector3(0.0f, 0.0f, 1.0f);
Vector3 Vector3::Backward = Vector3(0.0f, 0.0f, -1.0f);

Vector3::Vector3(const Vector2& v2, float z)
{
    x = v2.x;
    y = v2.y;
    z = z;
}

Vector3 Vector3::Lerp(Vector3 start, Vector3 end, float t)
{
    float x = Math::Lerp(start.x, end.x, t);
    float y = Math::Lerp(start.y, end.y, t);
    float z = Math::Lerp(start.z, end.z, t);

    return Vector3(x, y, z);
}