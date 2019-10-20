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
class Vector3;

class Vector4
{
public:
    // Constructors
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float inX, float inY, float inZ, float inW)
    {
        x = inX;
        y = inY;
        z = inZ;
        w = inW;
    }
    Vector4(float in)
    {
        x = in;
        y = in;
        z = in;
        w = in;
    }
    Vector4(unsigned char inX, unsigned char inY, unsigned char inZ, unsigned char inW)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
        z = static_cast<float>(inW);
    }
    Vector4(unsigned char in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
        w = static_cast<float>(in);
    }
    Vector4(unsigned short inX, unsigned short inY, float inZ, float inW)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
        w = static_cast<float>(inW);
    }
    Vector4(unsigned short in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
        w = static_cast<float>(in);
    }
    Vector4(unsigned int inX, unsigned int inY, float inZ, float inW)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
        z = static_cast<float>(inW);
    }
    Vector4(unsigned int in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
        w = static_cast<float>(in);
    }
    Vector4(signed char inX, signed char inY, float inZ, float inW)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
        w = static_cast<float>(inW);
    }
    Vector4(signed char in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
        w = static_cast<float>(in);
    }
    Vector4(signed short inX, signed short inY, float inZ, float inW)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
        w = static_cast<float>(inW);
    }
    Vector4(signed short in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
        w = static_cast<float>(in);
    }
    Vector4(signed int inX, signed int inY, float inZ, float inW)
    {
        x = static_cast<float>(inX);
        y = static_cast<float>(inY);
        z = static_cast<float>(inZ);
        w = static_cast<float>(inW);
    }
    Vector4(signed int in)
    {
        x = static_cast<float>(in);
        y = static_cast<float>(in);
        z = static_cast<float>(in);
        w = static_cast<float>(in);
    }

    Vector4(const Vector4& in)
    {
        x = in.x;
        y = in.y;
        z = in.z;
        w = in.w;
    }

    Vector4(const Vector3& in, float w = 0.0f);
    Vector4(const Vector2& in, float z = 0.0f, float w = 0.0f);

    // Returns the length of the vector
    inline float Length() const
    {
        return Math::Sqrt(SqrLength());
    }

    // Returns the squared length of the vector
    inline float SqrLength() const
    {
        return x * x + y * y + z * z + w * w;
    }

    // Returns the dot product of the vector and another vector
    inline float Dot(const Vector4& other) const
    {
        return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
    }

    // Returns the vector with a length of 1, does not modify the original
    inline Vector4 Normalize() const
    {
        float length = Length();
        return Vector4(x / length, y / length, z / length, w / length);
    }

    inline float Sum() const
    {
        return x + y + z + w;
    }

    // Returns the lerp result of the vector over T
    static Vector4 Lerp(Vector4 start, Vector4 end, float t);

    // Returns a nicely formatted string of the vector
    inline std::string ToString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
    }


    // Overloaded operators
    inline Vector4 operator+(const Vector4& other)
    {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    inline Vector4 operator+(const float other)
    {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator+(const unsigned char other)
    {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator+(const unsigned short other)
    {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator+(const unsigned int other)
    {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator+(const signed char other)
    {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator+(const signed short other)
    {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator+(const signed int other)
    {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator-(const Vector4& other)
    {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    inline Vector4 operator-(const float other)
    {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator-(const unsigned char other)
    {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator-(const unsigned short other)
    {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator-(const unsigned int other)
    {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator-(const signed char other)
    {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator-(const signed short other)
    {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator-(const signed int other)
    {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator*(const Vector4& other)
    {
        return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    inline Vector4 operator*(const float other)
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline Vector4 operator*(const unsigned char other)
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline Vector4 operator*(const unsigned short other)
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline Vector4 operator*(const unsigned int other)
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline Vector4 operator*(const signed char other)
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline Vector4 operator*(const signed short other)
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline Vector4 operator*(const signed int other)
    {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline Vector4 operator/(const Vector4& other)
    {
        return Vector4(x / other.x, y / other.y, z / other.z, w / other.w);
    }

    inline Vector4 operator/(const float other)
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    inline Vector4 operator/(const unsigned char other)
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    inline Vector4 operator/(const unsigned short other)
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    inline Vector4 operator/(const unsigned int other)
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    inline Vector4 operator/(const signed char other)
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    inline Vector4 operator/(const signed short other)
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    inline Vector4 operator/(const signed int other)
    {
        return Vector4(x / other, y / other, z / other, w / other);
    }

    inline Vector4 operator%(const Vector4& other)
    {
        return Vector4(Math::Modulus(x, other.x), Math::Modulus(y, other.y), Math::Modulus(z, other.z), Math::Modulus(w, other.w));
    }

    inline Vector4 operator%(const float other)
    {
        return Vector4(Math::Modulus(x, other), Math::Modulus(y, other), Math::Modulus(z, other), Math::Modulus(w, other));
    }

    inline Vector4 operator%(const unsigned char other)
    {
        return Vector4(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)), Math::Modulus(w, static_cast<float>(other)));
    }

    inline Vector4 operator%(const unsigned short other)
    {
        return Vector4(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)), Math::Modulus(w, static_cast<float>(other)));
    }

    inline Vector4 operator%(const unsigned int other)
    {
        return Vector4(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)), Math::Modulus(w, static_cast<float>(other)));
    }

    inline Vector4 operator%(const signed char other)
    {
        return Vector4(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)), Math::Modulus(w, static_cast<float>(other)));
    }

    inline Vector4 operator%(const signed short other)
    {
        return Vector4(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)), Math::Modulus(w, static_cast<float>(other)));
    }

    inline Vector4 operator%(const signed int other)
    {
        return Vector4(Math::Modulus(x, static_cast<float>(other)), Math::Modulus(y, static_cast<float>(other)), Math::Modulus(z, static_cast<float>(other)), Math::Modulus(w, static_cast<float>(other)));
    }

    // opAssign
    inline Vector4& operator+=(const Vector4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    inline Vector4& operator+=(const float other)
    {
        x += other;
        y += other;
        z += other;
        w += other;
        return *this;
    }

    inline Vector4& operator+=(const unsigned char other)
    {
        x += other;
        y += other;
        z += other;
        w += other;
        return *this;
    }

    inline Vector4& operator+=(const unsigned short other)
    {
        x += other;
        y += other;
        z += other;
        w += other;
        return *this;
    }

    inline Vector4& operator+=(const unsigned int other)
    {
        x += other;
        y += other;
        z += other;
        w += other;
        return *this;
    }

    inline Vector4& operator+=(const signed char other)
    {
        x += other;
        y += other;
        z += other;
        w += other;
        return *this;
    }

    inline Vector4& operator+=(const signed short other)
    {
        x += other;
        y += other;
        z += other;
        w += other;
        return *this;
    }

    inline Vector4& operator+=(const signed int other)
    {
        x += other;
        y += other;
        z += other;
        w += other;
        return *this;
    }

    inline Vector4& operator-=(const Vector4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    inline Vector4& operator-=(const float other)
    {
        x -= other;
        y -= other;
        z -= other;
        w -= other;
        return *this;
    }

    inline Vector4& operator-=(const unsigned char other)
    {
        x -= other;
        y -= other;
        z -= other;
        w -= other;
        return *this;
    }

    inline Vector4& operator-=(const unsigned short other)
    {
        x -= other;
        y -= other;
        z -= other;
        w -= other;
        return *this;
    }

    inline Vector4& operator-=(const unsigned int other)
    {
        x -= other;
        y -= other;
        z -= other;
        w -= other;
        return *this;
    }

    inline Vector4& operator-=(const signed char other)
    {
        x -= other;
        y -= other;
        z -= other;
        w -= other;
        return *this;
    }

    inline Vector4& operator-=(const signed short other)
    {
        x -= other;
        y -= other;
        z -= other;
        w -= other;
        return *this;
    }

    inline Vector4& operator-=(const signed int other)
    {
        x -= other;
        y -= other;
        z -= other;
        w -= other;
        return *this;
    }

    inline Vector4& operator*=(const Vector4& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }

    inline Vector4& operator*=(const float other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    inline Vector4& operator*=(const unsigned char other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    inline Vector4& operator*=(const unsigned short other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    inline Vector4& operator*=(const unsigned int other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    inline Vector4& operator*=(const signed char other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    inline Vector4& operator*=(const signed short other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    inline Vector4& operator*=(const signed int other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

    inline Vector4& operator/=(const Vector4& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }

    inline Vector4& operator/=(const float other)
    {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    inline Vector4& operator/=(const unsigned char other)
    {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    inline Vector4& operator/=(const unsigned short other)
    {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    inline Vector4& operator/=(const unsigned int other)
    {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    inline Vector4& operator/=(const signed char other)
    {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    inline Vector4& operator/=(const signed short other)
    {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    inline Vector4& operator/=(const signed int other)
    {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
        return *this;
    }

    // Comp
    bool operator==(const Vector4& other)
    {
        return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
    }

    bool operator!=(const Vector4& other)
    {
        return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
    }

public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
public:
    static Vector4 One;
    static Vector4 Zero;
};