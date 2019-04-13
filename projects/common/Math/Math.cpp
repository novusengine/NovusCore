#include "Math.h"
#include <cmath>

namespace Math
{
	f32 Sqrt(f32 in)
	{
		return sqrt(in);
	}

	f32 DegToRad(f32 deg)
	{
		return (deg * PI) / 180.0f;
	}

	f32 RadToDeg(f32 rad)
	{
		return (rad * 180.0f) / PI;
	}
}