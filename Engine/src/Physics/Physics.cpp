#include "Physics/Physics.h"

#include "ECS/Components/Components.h"

namespace Luden
{
	Math::Vec2 Physics::GetOverlap(const Entity& a, const Entity& b)
	{
		Math::Vec2 posA = a.Get<CTransform>().pos;
		Math::Vec2 posB = b.Get<CTransform>().pos;
		Math::Vec2 halfA = a.Get<CBoundingBox>().halfSize;
		Math::Vec2 halfB = b.Get<CBoundingBox>().halfSize;

		Math::Vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
		return halfA + halfB - delta;
	}

	Math::Vec2 Physics::GetPreviousOverlap(const Entity& a, const Entity& b)
	{
		Math::Vec2 posA = a.Get<CTransform>().prevPos;
		Math::Vec2 posB = b.Get<CTransform>().prevPos;
		Math::Vec2 halfA = a.Get<CBoundingBox>().halfSize;
		Math::Vec2 halfB = b.Get<CBoundingBox>().halfSize;

		Math::Vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
		return halfA + halfB - delta;
	}

	bool Physics::IsInside(const Math::Vec2& pos, const Entity& entity)
	{
		auto anim = entity.Get<CAnimation>().animation;
		Math::Vec2 size = anim.GetSize();
		Math::Vec2 ePos = entity.Get<CTransform>().pos;

		return (pos.x > ePos.x - size.x / 2 && 
				pos.x < ePos.x + size.x / 2 &&
				pos.y > ePos.y - size.y / 2 && 
				pos.y < ePos.y + size.y / 2
			);
	}

	Intersect Physics::LineIntersect(const Math::Vec2& a, const Math::Vec2& b, const Math::Vec2& c, const Math::Vec2& d)
	{
		Math::Vec2 r = b - a;
		Math::Vec2 s = d - c;
		float rxs = r.x * s.y - r.y * s.x;
		Math::Vec2 cma = c - a;

		if (rxs == 0.0f)
			return { false, Math::Vec2(0, 0) };

		float t = (cma.x * s.y - cma.y * s.x) / rxs;
		float u = (cma.x * r.y - cma.y * r.x) / rxs;

		if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
			return { true, a + t * r };
		else
			return { false, Math::Vec2(0, 0) };
	}

	bool Physics::EntityIntersect(const Math::Vec2& a, const Math::Vec2& b, const Entity& entity)
	{
		auto boxC = entity.Get<CBoundingBox>().center;
		auto box = entity.Get<CBoundingBox>().halfSize;

		Math::Vec2 e1{ boxC.x - box.x, boxC.y - box.y };
		Math::Vec2 e2{ boxC.x + box.x, boxC.y - box.y };
		Math::Vec2 e3{ boxC.x + box.x, boxC.y + box.y };
		Math::Vec2 e4{ boxC.x - box.x, boxC.y + box.y };

		return LineIntersect(a, b, e1, e2).hit ||
			   LineIntersect(a, b, e2, e3).hit ||
			   LineIntersect(a, b, e3, e4).hit ||
			   LineIntersect(a, b, e4, e1).hit;
	}

	RectOverlap Physics::AIsNearB(const Entity& a, const Entity& b, const Math::Vec2& maxDist)
	{
		ODirection dir = ODirection::NONE;
		Math::Vec2 overlap = GetOverlap(a, b);
		Math::Vec2 prevOverlap = GetPreviousOverlap(a, b);

		float dy = b.Get<CTransform>().pos.y - a.Get<CTransform>().pos.y;
		if (overlap.x > 0 && overlap.y > 0 && -maxDist.y < overlap.y && prevOverlap.y <= 0)
		{
			dir = dy > 0 ? ODirection::UP : ODirection::DOWN;
		}

		float dx = b.Get<CTransform>().pos.x - a.Get<CTransform>().pos.x;
		if (overlap.y > 0 && overlap.x > 0 && -maxDist.x < overlap.x && prevOverlap.x <= 0)
		{
			dir = dx > 0 ? ODirection::LEFT : ODirection::RIGHT;
		}

		return { dir, overlap };
	}

	Math::Vec2 Physics::GetSpeedAB(const Math::Vec2& posA, const Math::Vec2& posB, float speed)
	{
		float theta = std::atan2(posB.y - posA.y, posB.x - posA.x);
		return Math::Vec2(speed * std::cos(theta), speed * std::sin(theta));
	}
}
