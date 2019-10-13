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
#include <string>
#include "Math.h"

class Vector2;

// Right handed coordinate system, positive X points north, positive Y points west and positive Z points up
class Vector3
{
public:
    // Constructors
    Vector3() : x(0.0f), y(0.0f), z(0.0f){};
    Vector3(float inX, float inY, float inZ)
    {
        x = inX;
        y = inY;
        z = inZ;
    }
    Vector3(float in)
    {
        x = in;
        y = in;
        z = in;
    }
    Vector3(unsigned char inX, unsigned char inY, unsigned char inZ)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
    }
    Vector3(unsigned char in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
    }
    Vector3(unsigned short inX, unsigned short inY, float inZ)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
    }
    Vector3(unsigned short in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
    }
    Vector3(unsigned int inX, unsigned int inY, float inZ)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
    }
    Vector3(unsigned int in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
    }
    Vector3(signed char inX, signed char inY, float inZ)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
    }
    Vector3(signed char in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
    }
    Vector3(signed short inX, signed short inY, float inZ)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
    }
    Vector3(signed short in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
    }
    Vector3(signed int inX, signed int inY, float inZ)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
    }
    Vector3(signed int in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
    }

    Vector3(const Vector3& in)
    {
        x = in.x;
        y = in.y;
        z = in.z;
    }
    Vector3(const Vector2& in, float z = 0.0f);

    // Returns the length of the vector
    inline float Length()
    {
        return Math::Sqrt(SqrLength());
    }

    // Returns the squared length of the vector
    inline float SqrLength()
    {
        return x * x + y * y + z * z;
    }

    // Returns the dot product of the vector and another vector
    inline float Dot(const Vector3& other)
    {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }

    // Returns the vector with a length of 1, does not modify the original
    inline Vector3 Normalize()
    {
        float length = Length();
        return Vector3(x / length, y / length, z / length);
    }

    // Returns the lerp result of the vector over T
    static Vector3 Lerp(Vector3 start, Vector3 end, float t);

    // Returns a nicely formatted string of the vector
    inline std::string ToString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }

    // Overloaded operators
    inline Vector3 operator+(const Vector3& other)
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    inline Vector3 operator+(const float other)
    {
        return Vector3(x + other, y + other, z + other);
    }

    inline Vector3 operator+(const unsigned char other)
    {
        return Vector3(x + other, y + other, z + other);
    }

    inline Vector3 operator+(const unsigned short other)
    {
        return Vector3(x + other, y + other, z + other);
    }

    inline Vector3 operator+(const unsigned int other)
    {
        return Vector3(x + other, y + other, z + other);
    }

    inline Vector3 operator+(const signed char other)
    {
        return Vector3(x + other, y + other, z + other);
    }

    inline Vector3 operator+(const signed short other)
    {
        return Vector3(x + other, y + other, z + other);
    }

    inline Vector3 operator+(const signed int other)
    {
        return Vector3(x + other, y + other, z + other);
    }

    inline Vector3 operator-(const Vector3& other)
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    inline Vector3 operator-(const float other)
    {
        return Vector3(x - other, y - other, z - other);
    }

    inline Vector3 operator-(const unsigned char other)
    {
        return Vector3(x - other, y - other, z - other);
    }

    inline Vector3 operator-(const unsigned short other)
    {
        return Vector3(x - other, y - other, z - other);
    }

    inline Vector3 operator-(const unsigned int other)
    {
        return Vector3(x - other, y - other, z - other);
    }

    inline Vector3 operator-(const signed char other)
    {
        return Vector3(x - other, y - other, z - other);
    }

    inline Vector3 operator-(const signed short other)
    {
        return Vector3(x - other, y - other, z - other);
    }

    inline Vector3 operator-(const signed int other)
    {
        return Vector3(x - other, y - other, z - other);
    }

    inline Vector3 operator*(const Vector3& other)
    {
        return Vector3(x * other.x, y * other.y, z * other.z);
    }

    inline Vector3 operator*(const float other)
    {
        return Vector3(x * other, y * other, z * other);
    }

    inline Vector3 operator*(const unsigned char other)
    {
        return Vector3(x * other, y * other, z * other);
    }

    inline Vector3 operator*(const unsigned short other)
    {
        return Vector3(x * other, y * other, z * other);
    }

    inline Vector3 operator*(const unsigned int other)
    {
        return Vector3(x * other, y * other, z * other);
    }

    inline Vector3 operator*(const signed char other)
    {
        return Vector3(x * other, y * other, z * other);
    }

    inline Vector3 operator*(const signed short other)
    {
        return Vector3(x * other, y * other, z * other);
    }

    inline Vector3 operator*(const signed int other)
    {
        return Vector3(x * other, y * other, z * other);
    }

    inline Vector3 operator/(const Vector3& other)
    {
        return Vector3(x / other.x, y / other.y, z / other.z);
    }

    inline Vector3 operator/(const float other)
    {
        return Vector3(x / other, y / other, z / other);
    }

    inline Vector3 operator/(const unsigned char other)
    {
        return Vector3(x / other, y / other, z / other);
    }

    inline Vector3 operator/(const unsigned short other)
    {
        return Vector3(x / other, y / other, z / other);
    }

    inline Vector3 operator/(const unsigned int other)
    {
        return Vector3(x / other, y / other, z / other);
    }

    inline Vector3 operator/(const signed char other)
    {
        return Vector3(x / other, y / other, z / other);
    }

    inline Vector3 operator/(const signed short other)
    {
        return Vector3(x / other, y / other, z / other);
    }

    inline Vector3 operator/(const signed int other)
    {
        return Vector3(x / other, y / other, z / other);
    }

    inline Vector3 operator%(const Vector3& other)
    {
        return Vector3(Math::Modulus(x, other.x), Math::Modulus(y, other.y), Math::Modulus(z, other.z));
    }

    inline Vector3 operator%(const float other)
    {
        return Vector3(Math::Modulus(x, other), Math::Modulus(y, other), Math::Modulus(z, other));
    }

    inline Vector3 operator%(const unsigned char other)
    {
        return Vector3(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)));
    }

    inline Vector3 operator%(const unsigned short other)
    {
        return Vector3(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)));
    }

    inline Vector3 operator%(const unsigned int other)
    {
        return Vector3(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)));
    }

    inline Vector3 operator%(const signed char other)
    {
        return Vector3(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)));
    }

    inline Vector3 operator%(const signed short other)
    {
        return Vector3(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)));
    }

    inline Vector3 operator%(const signed int other)
    {
        return Vector3(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)));
    }

    // opAssign
    inline Vector3& operator+=(const Vector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    inline Vector3& operator+=(const float other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    inline Vector3& operator+=(const unsigned char other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    inline Vector3& operator+=(const unsigned short other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    inline Vector3& operator+=(const unsigned int other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    inline Vector3& operator+=(const signed char other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    inline Vector3& operator+=(const signed short other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    inline Vector3& operator+=(const signed int other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }

    inline Vector3& operator-=(const Vector3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        ;
        return *this;
    }

    inline Vector3& operator-=(const float other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    inline Vector3& operator-=(const unsigned char other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    inline Vector3& operator-=(const unsigned short other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    inline Vector3& operator-=(const unsigned int other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    inline Vector3& operator-=(const signed char other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    inline Vector3& operator-=(const signed short other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    inline Vector3& operator-=(const signed int other)
    {
        x -= other;
        y -= other;
        z -= other;
        return *this;
    }

    inline Vector3& operator*=(const Vector3& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    inline Vector3& operator*=(const float other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vector3& operator*=(const unsigned char other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vector3& operator*=(const unsigned short other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vector3& operator*=(const unsigned int other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vector3& operator*=(const signed char other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vector3& operator*=(const signed short other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vector3& operator*=(const signed int other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vector3& operator/=(const Vector3& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    inline Vector3& operator/=(const float other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    inline Vector3& operator/=(const unsigned char other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    inline Vector3& operator/=(const unsigned short other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    inline Vector3& operator/=(const unsigned int other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    inline Vector3& operator/=(const signed char other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    inline Vector3& operator/=(const signed short other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    inline Vector3& operator/=(const signed int other)
    {
        x /= other;
        y /= other;
        z /= other;
        return *this;
    }

    // Comp
    bool operator==(const Vector3& other)
    {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

public:
    static Vector3 One;
    static Vector3 Zero;

    static Vector3 Up;
    static Vector3 Down;
    static Vector3 Left;
    static Vector3 Right;
    static Vector3 Forward;
    static Vector3 Backward;
};