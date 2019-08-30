#include "Vector2.h"
#include "Vector3.h"

// Right handed coordinate system, positive X points north and positive Y points west
Vector2 Vector2::One(1, 1);
Vector2 Vector2::Zero(0, 0);
Vector2 Vector2::North(1, 0);
Vector2 Vector2::South(-1, 0);
Vector2 Vector2::West(0, 1);
Vector2 Vector2::East(0, -1);

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
