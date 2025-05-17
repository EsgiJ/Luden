#pragma once

#include "EngineAPI.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden::Math
{
	struct ENGINE_API Vec2 {
		float x = 0;
		float y = 0;

		static float epsilon;
		static const Vec2 kZero;

		Vec2();

		Vec2(float xin, float yin);


		Vec2 operator*(float val) const;

		Vec2 operator/(float val) const;

		Vec2 operator+=(Vec2 rhs);

		Vec2 operator-=(Vec2 rhs);

		Vec2 operator*=(float val);

		Vec2 operator/=(float val);


		bool operator==(const Vec2& rhs) const;

		bool operator!=(const Vec2& rhs) const;

		[[nodiscard]] float Length() const;

		[[nodiscard]] float LengthSquared() const;

		Vec2 Normalize();
	};

	struct ENGINE_API Point2 {
		float x = 0;
		float y = 0;

		static float epsilon;
		static const Point2 kOrigin;

		Point2();

		Point2(float xin, float yin);

		bool operator==(const Point2& rhs) const;

		bool operator!=(const Point2& rhs) const;

		[[nodiscard]] float Distance(Point2 p) const;

		[[nodiscard]] float DistanceSquared(Point2 p) const;
	};

	Vec2 operator+(Vec2 lhs, Vec2 rhs);

	Point2 operator+(Point2 lhs, Vec2 rhs);

	Vec2 operator-(Vec2 lhs, Vec2 rhs);

	Vec2 operator-(Point2 lhs, Point2 rhs);

	Vec2 operator*(float val, Vec2 lhs);

	bool Equals(Vec2 lhs, Vec2 rhs);

	bool Equals(Point2 lhs, Point2 rhs);

	float Length(Vec2 a);

	float LengthSquared(Vec2 a);

	Vec2 Normalize(Vec2 a);

	float Dot(Vec2 lhs, Vec2 rhs);

	float Distance(Point2 lhs, Point2 rhs);

	float DistanceSquared(Point2 lhs, Point2 rhs);

	float AngleRadians(Vec2 lhs, Vec2 rhs);

	float AngleDegrees(Vec2 lhs, Vec2 rhs);

	bool Clockwise(Vec2 lhs, Vec2 rhs);

	bool CounterClockwise(Vec2 lhs, Vec2 rhs);
}


