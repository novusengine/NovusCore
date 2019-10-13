#include "Vector2.h"
#include "Vector3.h"

// Right handed coordinate system, positive X points right, positive Y points up
Vector2 Vector2::One(1, 1);
Vector2 Vector2::Zero(0, 0);
Vector2 Vector2::Up(0, 1);
Vector2 Vector2::Down(0, -1);
Vector2 Vector2::Left(-1, 0);
Vector2 Vector2::Right(1, 0);

Vector2::Vector2(const Vector3& v3)
{
    x = v3.x;
    y = v3.y;
}

Vector2 Vector2::Lerp(Vector2 start, Vector2 end, float t)
{
    float x = Math::Lerp(start.x, start.x, t);
    float y = Math::Lerp(start.y, start.y, t);

    return Vector2(x, y);
}
