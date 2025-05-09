#include "Physics/Physics.h"

namespace Luden
{
	Math::Vec2 Physics::GetOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b)
	{
		Math::Vec2 posA = a->Get<CTransform>().pos;
		Math::Vec2 posB = b->Get<CTransform>().pos;
		Math::Vec2 halfA = a->Get<CBoundingBox>().halfSize;
		Math::Vec2 halfB = b->Get<CBoundingBox>().halfSize;

		Math::Vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
		return halfA + halfB - delta;
	}

	Math::Vec2 Physics::GetPreviousOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b)
	{
		Math::Vec2 posA = a->Get<CTransform>().prevPos;
		Math::Vec2 posB = b->Get<CTransform>().prevPos;
		Math::Vec2 halfA = a->Get<CBoundingBox>().halfSize;
		Math::Vec2 halfB = b->Get<CBoundingBox>().halfSize;

		Math::Vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
		return halfA + halfB - delta;
	}

	bool Physics::IsInside(const Math::Vec2& pos, const std::shared_ptr<Entity>& entity)
	{
		auto& anim = entity->Get<CAnimation>().animation;
		Math::Vec2 size = anim.GetSize();
		Math::Vec2 ePos = entity->Get<CTransform>().pos;

		return (pos.x > ePos.x - size.x / 2 && pos.x < ePos.x + size.x / 2 &&
			pos.y > ePos.y - size.y / 2 && pos.y < ePos.y + size.y / 2);
	}

	Intersect Physics::LineIntersect(const Math::Vec2& a, const Math::Vec2& b, const Math::Vec2& c, const Math::Vec2& d)
	{
		// You can implement your intersection logic here or keep it unimplemented if unused
		return { false, Math::Vec2(0, 0) };
	}

	bool Physics::EntityIntersect(const Math::Vec2& a, const Math::Vec2& b, const std::shared_ptr<Entity>& entity)
	{
		// You can implement your entity-line collision logic here or keep it unimplemented if unused
		return false;
	}
}
