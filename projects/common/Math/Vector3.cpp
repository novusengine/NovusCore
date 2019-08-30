#include "Vector3.h"
#include "Vector2.h"

// Right handed coordinate system, positive X points north, positive Y points west and positive Z points up
Vector3 Vector3::One(1, 1, 1);
Vector3 Vector3::Zero(0, 0, 0);
Vector3 Vector3::North(1, 0, 0);
Vector3 Vector3::South(-1, 0, 0);
Vector3 Vector3::West(0, 1, 0);
Vector3 Vector3::East(0, -1, 0);
Vector3 Vector3::Up(0, 0, 1);
Vector3 Vector3::Down(0, 0, -1);

Vector3::Vector3(const Vector2& v2)
{
    x = v2.x;
    y = v2.y;
    z = 0;
}

Vector3 Vector3::Lerp(Vector3 start, Vector3 end, float t)
{
    float x = Math::Lerp(start.x, end.x, t);
    float y = Math::Lerp(start.y, end.y, t);
    float z = Math::Lerp(start.z, end.z, t);

    return Vector3(x, y, z);
}