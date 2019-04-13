#pragma once
#include "../NovusTypes.h"
#include <string>

class Vector2;

// Right handed coordinate system, positive X points north, positive Y points west and positive Z points up
class Vector3
{
public:
	// Constructors
	Vector3() {};
	Vector3(f32 inX, f32 inY, f32 inZ) { x = inX; y = inY; z = inZ; }
	Vector3(f32 in) { x = in; y = in; z = in; }
	Vector3(u8 inX, u8 inY, u8 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	Vector3(u8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	Vector3(u16 inX, u16 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	Vector3(u16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	Vector3(u32 inX, u32 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	Vector3(u32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	Vector3(i8 inX, i8 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	Vector3(i8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	Vector3(i16 inX, i16 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	Vector3(i16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	Vector3(i32 inX, i32 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	Vector3(i32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }

	Vector3(Vector3& in) { x = in.x; y = in.y; z = in.z; }
	Vector3(Vector2& in);

	// Returns the length of the vector
	f32 Length();
	// Returns the squared length of the vector
	f32 SqrLength();
	// Returns the dot product of the vector and another vector
	f32 Dot(Vector3& other);
	// Returns the vector with a length of 1, does not modify the original
	Vector3 Normalize();
	// Returns a nicely formatted string of the vector
	std::string ToString();

	// Overloaded operators
	Vector3 operator+ (Vector3);
	Vector3 operator+ (f32);
	Vector3 operator+ (u8);
	Vector3 operator+ (u16);
	Vector3 operator+ (u32);
	Vector3 operator+ (i8);
	Vector3 operator+ (i16);
	Vector3 operator+ (i32);
	Vector3 operator- (Vector3);
	Vector3 operator- (f32);
	Vector3 operator- (u8);
	Vector3 operator- (u16);
	Vector3 operator- (u32);
	Vector3 operator- (i8);
	Vector3 operator- (i16);
	Vector3 operator- (i32);
	Vector3 operator* (Vector3);
	Vector3 operator* (f32);
	Vector3 operator* (u8);
	Vector3 operator* (u16);
	Vector3 operator* (u32);
	Vector3 operator* (i8);
	Vector3 operator* (i16);
	Vector3 operator* (i32);
	Vector3 operator/ (Vector3);
	Vector3 operator/ (f32);
	Vector3 operator/ (u8);
	Vector3 operator/ (u16);
	Vector3 operator/ (u32);
	Vector3 operator/ (i8);
	Vector3 operator/ (i16);
	Vector3 operator/ (i32);

public:
	f32 x = 0.0f;
	f32 y = 0.0f;
	f32 z = 0.0f;

public:
	static Vector3 One;
	static Vector3 Zero;
	static Vector3 North;
	static Vector3 South;
	static Vector3 West;
	static Vector3 East;
	static Vector3 Up;
	static Vector3 Down;
};