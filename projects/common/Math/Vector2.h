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

class Vector3;

// Right handed coordinate system, positive X points north and positive Y points west
class Vector2
{
public:
	// Constructors
	Vector2() {};
	Vector2(float x, float y) : x(x), y(y) {}

	// Vector2(f32 inX, f32 inY) { x = inX; y = inY; }
	// Vector2(f32 in) { x = in; y = in; }
	// Vector2(u8 inX, u8 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	// Vector2(u8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	// Vector2(u16 inX, u16 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	// Vector2(u16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	// Vector2(u32 inX, u32 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	// Vector2(u32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	// Vector2(i8 inX, i8 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	// Vector2(i8 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	// Vector2(i16 inX, i16 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	// Vector2(i16 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }
	// Vector2(i32 inX, i32 inY) { x = static_cast<f32>(inX); y = static_cast<f32>(inY); }
	// Vector2(i32 in) { x = static_cast<f32>(in); y = static_cast<f32>(in); }

	// Vector2(Vector2& in) { x = in.x; y = in.y; }
	Vector2(Vector3& in);

	// Returns the length of the vector
	inline f32 Length()
	{
		return Math::Sqrt(SqrLength());
	}

	// Returns the squared length of the vector
	inline f32 SqrLength()
	{
		return x * x + y * y;
	}

	// Returns the dot product of the vector and another vector
	inline f32 Dot(Vector2& other)
	{
		return (x * other.x) + (y * other.y);
	}

	// Returns the vector with a length of 1, does not modify the original
	inline Vector2 Normalize()
	{
		f32 length = Length();
		return Vector2(x / length, y / length);
	}

	// Returns a nicely formatted string of the vector
	inline std::string ToString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}

	// Overloaded operators
	inline Vector2 operator+ (Vector2 other)
	{
		return Vector2(x + other.x, y + other.y);
	}

	inline Vector2 operator+ (f32 other)
	{
		return Vector2(x + other, y + other);
	}

	inline Vector2 operator+ (u8 other)
	{
		return Vector2(x + other, y + other);
	}

	inline Vector2 operator+ (u16 other)
	{
		return Vector2(x + other, y + other);
	}

	inline Vector2 operator+ (u32 other)
	{
		return Vector2(x + other, y + other);
	}

	inline Vector2 operator+ (i8 other)
	{
		return Vector2(x + other, y + other);
	}

	inline Vector2 operator+ (i16 other)
	{
		return Vector2(x + other, y + other);
	}

	inline Vector2 operator+ (i32 other)
	{
		return Vector2(x + other, y + other);
	}

	inline Vector2 operator- (Vector2 other)
	{
		return Vector2(x - other.x, y - other.y);
	}

	inline Vector2 operator- (f32 other)
	{
		return Vector2(x - other, y - other);
	}

	inline Vector2 operator- (u8 other)
	{
		return Vector2(x - other, y - other);
	}

	inline Vector2 operator- (u16 other)
	{
		return Vector2(x - other, y - other);
	}

	inline Vector2 operator- (u32 other)
	{
		return Vector2(x - other, y - other);
	}

	inline Vector2 operator- (i8 other)
	{
		return Vector2(x - other, y - other);
	}

	inline Vector2 operator- (i16 other)
	{
		return Vector2(x - other, y - other);
	}

	inline Vector2 operator- (i32 other)
	{
		return Vector2(x - other, y - other);
	}

	inline Vector2 operator* (Vector2 other)
	{
		return Vector2(x * other.x, y * other.y);
	}

	inline Vector2 operator* (f32 other)
	{
		return Vector2(x * other, y * other);
	}

	inline Vector2 operator* (u8 other)
	{
		return Vector2(x * other, y * other);
	}

	inline Vector2 operator* (u16 other)
	{
		return Vector2(x * other, y * other);
	}

	inline Vector2 operator* (u32 other)
	{
		return Vector2(x * other, y * other);
	}

	inline Vector2 operator* (i8 other)
	{
		return Vector2(x * other, y * other);
	}

	inline Vector2 operator* (i16 other)
	{
		return Vector2(x * other, y * other);
	}

	inline Vector2 operator* (i32 other)
	{
		return Vector2(x * other, y * other);
	}

	inline Vector2 operator/ (Vector2 other)
	{
		return Vector2(x / other.x, y / other.y);
	}

	inline Vector2 operator/ (f32 other)
	{
		return Vector2(x / other, y / other);
	}

	inline Vector2 operator/ (u8 other)
	{
		return Vector2(x / other, y / other);
	}

	inline Vector2 operator/ (u16 other)
	{
		return Vector2(x / other, y / other);
	}

	inline Vector2 operator/ (u32 other)
	{
		return Vector2(x / other, y / other);
	}

	inline Vector2 operator/ (i8 other)
	{
		return Vector2(x / other, y / other);
	}

	inline Vector2 operator/ (i16 other)
	{
		return Vector2(x / other, y / other);
	}

	inline Vector2 operator/ (i32 other)
	{
		return Vector2(x / other, y / other);
	}

	inline Vector2 operator% (Vector2 other)
	{
		return Vector2(Math::Modulus(x, other.x), Math::Modulus(y, other.y));
	}

	inline Vector2 operator% (f32 other)
	{
		return Vector2(Math::Modulus(x, other), Math::Modulus(y, other));
	}

	inline Vector2 operator% (u8 other)
	{
		return Vector2(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)));
	}

	inline Vector2 operator% (u16 other)
	{
		return Vector2(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)));
	}

	inline Vector2 operator% (u32 other)
	{
		return Vector2(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)));
	}

	inline Vector2 operator% (i8 other)
	{
		return Vector2(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)));
	}

	inline Vector2 operator% (i16 other)
	{
		return Vector2(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)));
	}

	inline Vector2 operator% (i32 other)
	{
		return Vector2(Math::Modulus(x, static_cast<f32>(other)), Math::Modulus(y, static_cast<f32>(other)));
	}

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