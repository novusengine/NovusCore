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
	
};