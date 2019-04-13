#pragma once
#include "../NovusTypes.h"
#include <string>

class Vector3;

// Right handed coordinate system, positive X points north and positive Y points west
class Vector2
{
public:
	// Constructors
	Vector2() {};
	Vector2(f32 inX, f32 inY) { x = inX; y = inY; }
	Vector2(f32 in) { x = in; y = in; }
	Vector2(u8 inX, u8 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	Vector2(u8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	Vector2(u16 inX, u16 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	Vector2(u16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	Vector2(u32 inX, u32 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	Vector2(u32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	Vector2(i8 inX, i8 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	Vector2(i8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	Vector2(i16 inX, i16 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	Vector2(i16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	Vector2(i32 inX, i32 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	Vector2(i32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }

	Vector2(Vector2& in) { x = in.x; y = in.y; }
	Vector2(Vector3& in);

	// Returns the length of the vector
	inline f32 Length();
	// Returns the squared length of the vector
	inline f32 SqrLength();
	// Returns the dot product of the vector and another vector
	inline f32 Dot(Vector2& other);
	// Returns the vector with a length of 1, does not modify the original
	inline Vector2 Normalize();
	// Returns a nicely formatted string of the vector
	inline std::string ToString();

	// Overloaded operators
	Vector2 operator+ (Vector2);
	Vector2 operator+ (f32);
	Vector2 operator+ (u8);
	Vector2 operator+ (u16);
	Vector2 operator+ (u32);
	Vector2 operator+ (i8);
	Vector2 operator+ (i16);
	Vector2 operator+ (i32);
	Vector2 operator- (Vector2);
	Vector2 operator- (f32);
	Vector2 operator- (u8);
	Vector2 operator- (u16);
	Vector2 operator- (u32);
	Vector2 operator- (i8);
	Vector2 operator- (i16);
	Vector2 operator- (i32);
	Vector2 operator* (Vector2);
	Vector2 operator* (f32);
	Vector2 operator* (u8);
	Vector2 operator* (u16);
	Vector2 operator* (u32);
	Vector2 operator* (i8);
	Vector2 operator* (i16);
	Vector2 operator* (i32);
	Vector2 operator/ (Vector2);
	Vector2 operator/ (f32);
	Vector2 operator/ (u8);
	Vector2 operator/ (u16);
	Vector2 operator/ (u32);
	Vector2 operator/ (i8);
	Vector2 operator/ (i16);
	Vector2 operator/ (i32);

public:
	f32 x = 0.0f;
	f32 y = 0.0f;

public:
	static Vector2 One;
	static Vector2 Zero;
	static Vector2 North;
	static Vector2 South;
	static Vector2 West;
	static Vector2 East;
};