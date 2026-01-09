#include "ScriptAPI/MathAPI.h"

#include <glm/glm.hpp>

#include <iostream>
#include <random>

namespace Luden
{
	namespace MathAPI
	{
		static std::mt19937& GetRandomGenerator()
		{
			static std::mt19937 generator;
			static bool initialized = false;

			if (!initialized)
			{
				std::random_device rd;
				generator.seed(rd());
				initialized = true;
			}

			return generator;
		}

		float Lerp(float a, float b, float t)
		{
			return a + (b - a) * Clamp(t, 0.0f, 1.0f);
		}

		Vec2 Lerp(const Vec2& a, const Vec2& b, float t)
		{
			t = Clamp(t, 0.0f, 1.0f);
			return a + (b - a) * t;
		}

		Vec3 Lerp(const Vec3& a, const Vec3& b, float t)
		{
			t = Clamp(t, 0.0f, 1.0f);
			return a + (b - a) * t;
		}

		float Clamp(float value, float min, float max)
		{
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}

		float RandomFloat(float min, float max)
		{
			std::uniform_real_distribution<float> dist(min, max);
			return dist(GetRandomGenerator());
		}

		int RandomInt(int min, int max)
		{
			std::uniform_int_distribution<int> dist(min, max);
			return dist(GetRandomGenerator());
		}

		bool RandomBool(float probability)
		{
			return RandomFloat() < probability;
		}

		float Length(const Vec2& vec)
		{
			return glm::length(vec);	
		}

		float Length(const Vec3& vec)
		{
			return glm::length(vec);
		}

		Vec2 Normalize(const Vec2& vec)
		{
			return glm::normalize(vec);
		}

		Vec3 Normalize(const Vec3& vec)
		{
			return glm::normalize(vec);
		}
	}
}