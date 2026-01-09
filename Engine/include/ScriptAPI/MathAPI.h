#pragma once
#include "EngineAPI.h"
#include "Luden.h"  

namespace Luden
{
	namespace MathAPI
	{
		ENGINE_API float Lerp(float a, float b, float t);
		ENGINE_API Vec2 Lerp(const Vec2& a, const Vec2& b, float t);
		ENGINE_API Vec3 Lerp(const Vec3& a, const Vec3& b, float t);
		ENGINE_API float Clamp(float value, float min, float max);

		ENGINE_API float RandomFloat(float min = 0.0f, float max = 1.0f);
		ENGINE_API int RandomInt(int min, int max);
		ENGINE_API bool RandomBool(float probability = 0.5f);
	}
}