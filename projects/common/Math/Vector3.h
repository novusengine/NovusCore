/*
	MIT License

	Copyright (c) 2018-2019 NovusCore

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#pragma once
#include "../NovusTypes.h"
#include <string>
#include "Math.h"

class Vector2;

// Right handed coordinate system, positive X points north, positive Y points west and positive Z points up
class Vector3
{
public:
	// Constructors
	Vector3() {};
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	// Vector3(f32 inX, f32 inY, f32 inZ) { x = inX; y = inY; z = inZ; }
	// Vector3(f32 in) { x = in; y = in; z = in; }
	// Vector3(u8 inX, u8 inY, u8 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	// Vector3(u8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	// Vector3(u16 inX, u16 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	// Vector3(u16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	// Vector3(u32 inX, u32 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	// Vector3(u32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	// Vector3(i8 inX, i8 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	// Vector3(i8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	// Vector3(i16 inX, i16 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	// Vector3(i16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }
	// Vector3(i32 inX, i32 inY, f32 inZ) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); z = static_cast<f32>(inZ); }
	// Vector3(i32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); z = static_cast<f32>(in); }

	// Vector3(Vector3& in) { x = in.x; y = in.y; z = in.z; }
	Vector3(Vector2& in);

	// Returns the length of the vector
	inline f32 Length()
	{
		return Math::Sqrt(SqrLength());
	}

	// Returns the squared length of the vector
	inline f32 SqrLength()
	{
		return x * x + y * y + z * z;
	}

	// Returns the dot product of the vector and another vector
	inline f32 Dot(Vector3& other)
	{
		return (x * other.x) + (y * other.y) + (z * other.z);
	}

	// Returns the vector with a length of 1, does not modify the original
	inline Vector3 Normalize()
	{
		f32 length = Length();
		return Vector3(x / length, y / length, z / length);
	}
	// Returns a nicely formatted string of the vector
	inline std::string ToString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}

	// Overloaded operators
	inline Vector3 operator+ (Vector3 other)
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	inline Vector3 operator+ (f32 other)
	{
		return Vector3(x + other, y + other, z + other);
	}

	inline Vector3 operator+ (u8 other)
	{
		return Vector3(x + other, y + other, z + other);
	}

	inline Vector3 operator+ (u16 other)
	{
		return Vector3(x + other, y + other, z + other);
	}

	inline Vector3 operator+ (u32 other)
	{
		return Vector3(x + other, y + other, z + other);
	}

	inline Vector3 operator+ (i8 other)
	{
		return Vector3(x + other, y + other, z + other);
	}

	inline Vector3 operator+ (i16 other)
	{
		return Vector3(x + other, y + other, z + other);
	}

	inline Vector3 operator+ (i32 other)
	{
		return Vector3(x + other, y + other, z + other);
	}

	inline Vector3 operator- (Vector3 other)
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	inline Vector3 operator- (f32 other)
	{
		return Vector3(x - other, y - other, z - other);
	}

	inline Vector3 operator- (u8 other)
	{
		return Vector3(x - other, y - other, z - other);
	}

	inline Vector3 operator- (u16 other)
	{
		return Vector3(x - other, y - other, z - other);
	}

	inline Vector3 operator- (u32 other)
	{
		return Vector3(x - other, y - other, z - other);
	}

	inline Vector3 operator- (i8 other)
	{
		return Vector3(x - other, y - other, z - other);
	}

	inline Vector3 operator- (i16 other)
	{
		return Vector3(x - other, y - other, z - other);
	}

	inline Vector3 operator- (i32 other)
	{
		return Vector3(x - other, y - other, z - other);
	}

	inline Vector3 operator* (Vector3 other)
	{
		return Vector3(x * other.x, y * other.y, z * other.z);
	}

	inline Vector3 operator* (f32 other)
	{
		return Vector3(x * other, y * other, z * other);
	}

	inline Vector3 operator* (u8 other)
	{
		return Vector3(x * other, y * other, z * other);
	}

	inline Vector3 operator* (u16 other)
	{
		return Vector3(x * other, y * other, z * other);
	}

	inline Vector3 operator* (u32 other)
	{
		return Vector3(x * other, y * other, z * other);
	}

	inline Vector3 operator* (i8 other)
	{
		return Vector3(x * other, y * other, z * other);
	}

	inline Vector3 operator* (i16 other)
	{
		return Vector3(x * other, y * other, z * other);
	}

	inline Vector3 operator* (i32 other)
	{
		return Vector3(x * other, y * other, z * other);
	}

	inline Vector3 operator/ (Vector3 other)
	{
		return Vector3(x / other.x, y / other.y, z / other.z);
	}

	inline Vector3 operator/ (f32 other)
	{
		return Vector3(x / other, y / other, z / other);
	}

	inline Vector3 operator/ (u8 other)
	{
		return Vector3(x / other, y / other, z / other);
	}

	inline Vector3 operator/ (u16 other)
	{
		return Vector3(x / other, y / other, z / other);
	}

	inline Vector3 operator/ (u32 other)
	{
		return Vector3(x / other, y / other, z / other);
	}

	inline Vector3 operator/ (i8 other)
	{
		return Vector3(x / other, y / other, z / other);
	}

	inline Vector3 operator/ (i16 other)
	{
		return Vector3(x / other, y / other, z / other);
	}

	inline Vector3 operator/ (i32 other)
	{
		return Vector3(x / other, y / other, z / other);
	}

	inline Vector3 operator% (Vector3 other)
	{
		return Vector3(Math::Modulus(x, other.x), Math::Modulus(y,other.y), Math::Modulus(z, other.z));
	}

	inline Vector3 operator% (f32 other)
	{
		return Vector3(Math::Modulus(x, other), Math::Modulus(y, other), Math::Modulus(z, other));
	}

	inline Vector3 operator% (u8 other)
	{
		return Vector3(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)), Math::Modulus(z, static_cast<f32>(other)));
	}

	inline Vector3 operator% (u16 other)
	{
		return Vector3(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)), Math::Modulus(z, static_cast<f32>(other)));
	}

	inline Vector3 operator% (u32 other)
	{
		return Vector3(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)), Math::Modulus(z, static_cast<f32>(other)));
	}

	inline Vector3 operator% (i8 other)
	{
		return Vector3(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)), Math::Modulus(z, static_cast<f32>(other)));
	}

	inline Vector3 operator% (i16 other)
	{
		return Vector3(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)), Math::Modulus(z, static_cast<f32>(other)));
	}

	inline Vector3 operator% (i32 other)
	{
		return Vector3(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)), Math::Modulus(z, static_cast<f32>(other)));
	}

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