#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

// Right handed coordinate system, positive X points north, positive Y points west and positive Z points up
Vector4 Vector4::One(1, 1, 1, 1);
Vector4 Vector4::Zero(0, 0, 0, 0);

Vector4::Vector4(const Vector3& v3, float w)
{
    x = v3.x;
    y = v3.y;
    z = v3.z;
    w = w;
}

Vector4::Vector4(const Vector2& v2, float z, float w)
{
    x = v2.x;
    y = v2.y;
    z = z;
    w = w;
}

Vector4 Vector4::Lerp(Vector4 start, Vector4 end, float t)
{
    float x = Math::Lerp(start.x, end.x, t);
    float y = Math::Lerp(start.y, end.y, t);
    float z = Math::Lerp(start.z, end.z, t);
    float w = Math::Lerp(start.w, end.w, t);

    return Vector4(x, y, z, w);
}