#include "Vector2.h"
#include "Vector3.h"
#include "Math.h"

// Right handed coordinate system, positive X points north and positive Y points west
Vector2 Vector2::One(1, 1);
Vector2 Vector2::Zero(0, 0);
Vector2 Vector2::North(1, 0);
Vector2 Vector2::South(-1, 0);
Vector2 Vector2::West(0, 1);
Vector2 Vector2::East(0, -1);

inline Vector2::Vector2(Vector3& v3)
{
	x = v3.x;
	y = v3.y;
}

// Functions
inline f32 Vector2::Length()
{
	return Math::Sqrt(SqrLength());
}

inline f32 Vector2::SqrLength()
{
	return x * x + y * y;
}

inline f32 Vector2::Dot(Vector2& other)
{
	return (x * other.x) + (y * other.y);
}

inline Vector2 Vector2::Normalize()
{
	f32 length = Length();
	return Vector2(x / length, y / length);
}

inline std::string Vector2::ToString()
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

// Overloaded operators
// Addition
Vector2 Vector2::operator+(Vector2 other)
{
	return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator+(f32 other)
{
	return Vector2(x + other, y + other);
}

Vector2 Vector2::operator+(u8 other)
{
	return Vector2(x + other, y + other);
}

Vector2 Vector2::operator+(u16 other)
{
	return Vector2(x + other, y + other);
}

Vector2 Vector2::operator+(u32 other)
{
	return Vector2(x + other, y + other);
}

Vector2 Vector2::operator+(i8 other)
{
	return Vector2(x + other, y + other);
}

Vector2 Vector2::operator+(i16 other)
{
	return Vector2(x + other, y + other);
}

Vector2 Vector2::operator+(i32 other)
{
	return Vector2(x + other, y + other);
}

// Subtraction
Vector2 Vector2::operator-(Vector2 other)
{
	return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator-(f32 other)
{
	return Vector2(x - other, y - other);
}

Vector2 Vector2::operator-(u8 other)
{
	return Vector2(x - other, y - other);
}

Vector2 Vector2::operator-(u16 other)
{
	return Vector2(x - other, y - other);
}

Vector2 Vector2::operator-(u32 other)
{
	return Vector2(x - other, y - other);
}

Vector2 Vector2::operator-(i8 other)
{
	return Vector2(x - other, y - other);
}

Vector2 Vector2::operator-(i16 other)
{
	return Vector2(x - other, y - other);
}

Vector2 Vector2::operator-(i32 other)
{
	return Vector2(x - other, y - other);
}

// Multiplication
Vector2 Vector2::operator*(Vector2 other)
{
	return Vector2(x * other.x, y * other.y);
}

Vector2 Vector2::operator*(f32 other)
{
	return Vector2(x * other, y * other);
}

Vector2 Vector2::operator*(u8 other)
{
	return Vector2(x * other, y * other);
}

Vector2 Vector2::operator*(u16 other)
{
	return Vector2(x * other, y * other);
}

Vector2 Vector2::operator*(u32 other)
{
	return Vector2(x * other, y * other);
}

Vector2 Vector2::operator*(i8 other)
{
	return Vector2(x * other, y * other);
}

Vector2 Vector2::operator*(i16 other)
{
	return Vector2(x * other, y * other);
}

Vector2 Vector2::operator*(i32 other)
{
	return Vector2(x * other, y * other);
}

// Division
Vector2 Vector2::operator/(Vector2 other)
{
	return Vector2(x / other.x, y / other.y);
}

Vector2 Vector2::operator/(f32 other)
{
	return Vector2(x / other, y / other);
}

Vector2 Vector2::operator/(u8 other)
{
	return Vector2(x / other, y / other);
}

Vector2 Vector2::operator/(u16 other)
{
	return Vector2(x / other, y / other);
}

Vector2 Vector2::operator/(u32 other)
{
	return Vector2(x / other, y / other);
}

Vector2 Vector2::operator/(i8 other)
{
	return Vector2(x / other, y / other);
}

Vector2 Vector2::operator/(i16 other)
{
	return Vector2(x / other, y / other);
}

Vector2 Vector2::operator/(i32 other)
{
	return Vector2(x / other, y / other);
}