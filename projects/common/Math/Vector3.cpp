#include "Vector3.h"
#include "Vector2.h"
#include "Math.h"

// Right handed coordinate system, positive X points north, positive Y points west and positive Z points up
Vector3 Vector3::One(1, 1, 1);
Vector3 Vector3::Zero(0, 0, 0);
Vector3 Vector3::North(1, 0, 0);
Vector3 Vector3::South(-1, 0, 0);
Vector3 Vector3::West(0, 1, 0);
Vector3 Vector3::East(0, -1, 0);
Vector3 Vector3::Up(0, 0, 1);
Vector3 Vector3::Down(0, 0, -1);

inline Vector3::Vector3(Vector2& v2)
{
	x = v2.x;
	y = v2.y;
	z = 0;
}

// Functions
f32 Vector3::Length()
{
	return Math::Sqrt(SqrLength());
}

f32 Vector3::SqrLength()
{
	return x * x + y * y + z * z;
}

f32 Vector3::Dot(Vector3& other)
{
	return (x * other.x) + (y * other.y) + (z * other.z);
}

Vector3 Vector3::Normalize()
{
	f32 length = Length();
	return Vector3(x / length, y / length, z / length);
}

std::string Vector3::ToString()
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

// Overloaded operators
// Addition
Vector3 Vector3::operator+(Vector3 other)
{
	return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator+(f32 other)
{
	return Vector3(x + other, y + other, z + other);
}

Vector3 Vector3::operator+(u8 other)
{
	return Vector3(x + other, y + other, z + other);
}

Vector3 Vector3::operator+(u16 other)
{
	return Vector3(x + other, y + other, z + other);
}

Vector3 Vector3::operator+(u32 other)
{
	return Vector3(x + other, y + other, z + other);
}

Vector3 Vector3::operator+(i8 other)
{
	return Vector3(x + other, y + other, z + other);
}

Vector3 Vector3::operator+(i16 other)
{
	return Vector3(x + other, y + other, z + other);
}

Vector3 Vector3::operator+(i32 other)
{
	return Vector3(x + other, y + other, z + other);
}

// Subtraction
Vector3 Vector3::operator-(Vector3 other)
{
	return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator-(f32 other)
{
	return Vector3(x - other, y - other, z - other);
}

Vector3 Vector3::operator-(u8 other)
{
	return Vector3(x - other, y - other, z - other);
}

Vector3 Vector3::operator-(u16 other)
{
	return Vector3(x - other, y - other, z - other);
}

Vector3 Vector3::operator-(u32 other)
{
	return Vector3(x - other, y - other, z - other);
}

Vector3 Vector3::operator-(i8 other)
{
	return Vector3(x - other, y - other, z - other);
}

Vector3 Vector3::operator-(i16 other)
{
	return Vector3(x - other, y - other, z - other);
}

Vector3 Vector3::operator-(i32 other)
{
	return Vector3(x - other, y - other, z - other);
}

// Multiplication
Vector3 Vector3::operator*(Vector3 other)
{
	return Vector3(x * other.x, y * other.y, z * other.z);
}

Vector3 Vector3::operator*(f32 other)
{
	return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator*(u8 other)
{
	return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator*(u16 other)
{
	return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator*(u32 other)
{
	return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator*(i8 other)
{
	return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator*(i16 other)
{
	return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator*(i32 other)
{
	return Vector3(x * other, y * other, z * other);
}

// Division
Vector3 Vector3::operator/(Vector3 other)
{
	return Vector3(x / other.x, y / other.y, z / other.z);
}

Vector3 Vector3::operator/(f32 other)
{
	return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator/(u8 other)
{
	return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator/(u16 other)
{
	return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator/(u32 other)
{
	return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator/(i8 other)
{
	return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator/(i16 other)
{
	return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator/(i32 other)
{
	return Vector3(x / other, y / other, z / other);
}