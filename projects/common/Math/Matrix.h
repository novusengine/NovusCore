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
#include "Vector3.h"
#include "Vector4.h"

// This is a 4x4 Matrix, if we ever need other kinds we can refactor
class Matrix
{
public:
    enum MultiplicationType
    {
        PRE,	/**< Multiply the new matrix before the current matrix */
        POST,	/**< Multiply the new matrix after the current matrix */
        REPLACE	/**< Replace the current matrix with the new one */
    };

public:
    Matrix(bool identity = true);
    Matrix(const Matrix& other);

    union
    {
        struct
        {
            Vector4 row[4];
        };
        struct
        {
            Vector3 right;
            float pad1;
            Vector3 up;
            float pad2;
            Vector3 at;
            float pad3;
            Vector3 pos;
            float pad4;
        };
        struct
        {
            float element[16];
        };
    };

    Matrix& operator=(const Matrix& other);

    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const float scalar) const;
    Matrix operator+(const Matrix& other) const;
    Matrix operator/(const float scalar) const;

    Matrix& operator*=(const Matrix& other);
    Matrix& operator*=(const float scalar);
    Matrix& operator+=(const Matrix& other);
    Matrix& operator/=(const float scalar);
    
    Matrix& Transpose();
    Matrix Transposed() const;

    Matrix& Invert();
    Matrix Inverted() const;

    Matrix& RotateX(float angle, MultiplicationType mulType);
    Matrix& RotateY(float angle, MultiplicationType mulType);
    Matrix& RotateZ(float angle, MultiplicationType mulType);

    Matrix& Scale(Vector3 scale, MultiplicationType mulType);
    Matrix& Scale(float scale, MultiplicationType mulType);
    Matrix& Transform(const Matrix& transformation, MultiplicationType mulType);
    void TransformVector(Vector3& vector, bool translation) const;
};
