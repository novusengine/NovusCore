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

namespace Math
{
	const f32 PI = 3.1415926535f;
	const f32 TAU = 6.2831853071f; // Clearly superior

	f32 Sqrt(f32 in);
	
	inline f32 DegToRad(f32 deg)
	{
		return (deg * PI) / 180.0f;
	}

	inline f32 RadToDeg(f32 rad)
	{
		return (rad * 180.0f) / PI;
	}

	inline u32 FloorToInt(f32 x)
	{
		int xi = (int)x;
		return x < xi ? xi - 1 : xi;
	}

	inline f32 Floor(f32 x)
	{
		return static_cast<f32>(FloorToInt(x));
	}

	inline f32 Modulus(f32 a, f32 b)
	{
		return (a - b * Floor(a / b));
	}

	inline f32 Abs(f32 x)
	{
		return x < 0 ? -x : x;
	}
	
};