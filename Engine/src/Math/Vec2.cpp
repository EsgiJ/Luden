#include <cmath>
#include "Math/Vec2.h"

namespace Luden::Math
{
	float Vec2::epsilon = 0.001f;

	const Vec2 Vec2::kZero{ 0.0f, 0.0f };

	Vec2::Vec2() = default;

	Vec2::Vec2(float xin, float yin) : x(xin), y(yin) {}

	Vec2 Vec2::operator*(const float val) const {
		return { x * val, y * val };
	}

	Vec2 Vec2::operator/(const float val) const {
		return { x / val, y / val };
	}

	Vec2 Vec2::operator+=(Vec2 rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vec2 Vec2::operator-=(Vec2 rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vec2 Vec2::operator*=(const float val) {
		x *= val;
		y *= val;
		return *this;
	}

	Vec2 Vec2::operator/=(const float val) {
		x /= val;
		y /= val;
		return *this;
	}

	bool Vec2::operator==(const Vec2& rhs) const {
		return x == rhs.x && y == rhs.y;
	}

	bool Vec2::operator!=(const Vec2& rhs) const {
		return x != rhs.x || y != rhs.y;
	}

	float Vec2::Length() const {
		return std::sqrt(x * x + y * y);
	}

	float Vec2::LengthSquared() const {
		return x * x + y * y;
	}

	Vec2 Vec2::Normalize() {
		float l = Length();
		x /= l;
		y /= l;
		return *this;
	}

	float Point2::epsilon = 0.001f;

	const Point2 Point2::kOrigin{ 0.0f, 0.0f };

	Point2::Point2() = default;

	Point2::Point2(float xin, float yin) : x(xin), y(yin) {}

	bool Point2::operator==(const Point2& rhs) const {
		return x == rhs.x && y == rhs.y;
	}

	bool Point2::operator!=(const Point2& rhs) const {
		return x != rhs.x || y != rhs.y;
	}

	float Point2::Distance(Point2 p) const {
		// float dx = rhs.x - x;
		// float dy = rhs.y - y;
		// return std::sqrt(dx * dx + dy * dy);
		return (p - *this).Length();
	}

	float Point2::DistanceSquared(Point2 p) const {
		return (p - *this).LengthSquared();
	}

	Vec2 operator+(Vec2 lhs, Vec2 rhs) {
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	Point2 operator+(Point2 lhs, Vec2 rhs) {
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	Vec2 operator-(Vec2 lhs, Vec2 rhs) {
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	Vec2 operator-(Point2 lhs, Point2 rhs) {
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	Vec2 operator*(float val, Vec2 lhs) {
		return { lhs.x * val, lhs.y * val };
	}

	bool Equals(Vec2 lhs, Vec2 rhs) {
		float dx = std::abs(lhs.x - rhs.x);
		float dy = std::abs(lhs.y - rhs.y);
		return dx <= Vec2::epsilon && dy <= Vec2::epsilon;
	}

	bool Equals(Point2 lhs, Point2 rhs) {
		float dx = std::abs(lhs.x - rhs.x);
		float dy = std::abs(lhs.y - rhs.y);
		return dx <= Point2::epsilon && dy <= Point2::epsilon;
	}

	float Length(Vec2 a) {
		return a.Length();
	}

	float LengthSquared(Vec2 a) {
		return a.LengthSquared();
	}

	Vec2 Normalize(Vec2 a) {
		a.Normalize();
		return a;
	}

	float Dot(Vec2 lhs, Vec2 rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	float Distance(Point2 lhs, Point2 rhs) {
		return (lhs - rhs).Length();
	}

	float DistanceSquared(Point2 lhs, Point2 rhs) {
		return (lhs - rhs).LengthSquared();
	}

	float AngleRadians(Vec2 lhs, Vec2 rhs) {
		if (lhs == rhs) return 0.0;

		lhs.Normalize();
		rhs.Normalize();

		float d = Dot(lhs, rhs);
		if (std::abs(d - 1.0f) < Vec2::epsilon) {
			d = 1.0f;
		}

		return std::acos(d);
	}

	float AngleDegrees(Vec2 lhs, Vec2 rhs) {
		float a = AngleRadians(lhs, rhs);
		return a * 180.0f / 3.14159265359f;
	}

	bool Clockwise(Vec2 lhs, Vec2 rhs) {
		float cross = lhs.x * rhs.y - lhs.y * rhs.x;
		return cross < 0;
	}

	bool CounterClockwise(Vec2 lhs, Vec2 rhs) {
		float cross = lhs.x * rhs.y - lhs.y * rhs.x;
		return cross >= 0;
	}
}
