#include "Physics/Physics.h"

#include "Project/Project.h"
#include "ECS/Components/Components.h"

namespace Luden
{
	glm::vec2 Physics::GetOverlap(const Entity& a, const Entity& b)
	{
		glm::vec2 posA = a.Get<TransformComponent>().pos;
		glm::vec2 posB = b.Get<TransformComponent>().pos;
		glm::vec2 halfA = a.Get<BoxCollider2DComponent>().halfSize;
		glm::vec2 halfB = b.Get<BoxCollider2DComponent>().halfSize;

		glm::vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
		return halfA + halfB - delta;
	}

	glm::vec2 Physics::GetPreviousOverlap(const Entity& a, const Entity& b)
	{
		glm::vec2 posA = a.Get<TransformComponent>().prevPos;
		glm::vec2 posB = b.Get<TransformComponent>().prevPos;
		glm::vec2 halfA = a.Get<BoxCollider2DComponent>().halfSize;
		glm::vec2 halfB = b.Get<BoxCollider2DComponent>().halfSize;

		glm::vec2 delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
		return halfA + halfB - delta;
	}

	bool Physics::IsInside(const glm::vec2& pos, const Entity& entity)
	{
		auto cAnim = entity.Get<Animation2DComponent>();
		auto anim = std::static_pointer_cast<Graphics::Animation>(Project::GetResourceManager()->GetResource(cAnim.animationHandle));

		glm::vec2 size = anim->GetSize();
		glm::vec2 ePos = entity.Get<TransformComponent>().pos;

		return (pos.x > ePos.x - size.x / 2 && 
				pos.x < ePos.x + size.x / 2 &&
				pos.y > ePos.y - size.y / 2 && 
				pos.y < ePos.y + size.y / 2
			);
	}

	Intersect Physics::LineIntersect(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d)
	{
		glm::vec2 r = b - a;
		glm::vec2 s = d - c;
		float rxs = r.x * s.y - r.y * s.x;
		glm::vec2 cma = c - a;

		if (rxs == 0.0f)
			return { false, glm::vec2(0, 0) };

		float t = (cma.x * s.y - cma.y * s.x) / rxs;
		float u = (cma.x * r.y - cma.y * r.x) / rxs;

		if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
			return { true, a + t * r };
		else
			return { false, glm::vec2(0, 0) };
	}

	bool Physics::EntityIntersect(const glm::vec2& a, const glm::vec2& b, const Entity& entity)
	{
		auto boxC = entity.Get<BoxCollider2DComponent>().center;
		auto box = entity.Get<BoxCollider2DComponent>().halfSize;

		glm::vec2 e1{ boxC.x - box.x, boxC.y - box.y };
		glm::vec2 e2{ boxC.x + box.x, boxC.y - box.y };
		glm::vec2 e3{ boxC.x + box.x, boxC.y + box.y };
		glm::vec2 e4{ boxC.x - box.x, boxC.y + box.y };

		return LineIntersect(a, b, e1, e2).hit ||
			   LineIntersect(a, b, e2, e3).hit ||
			   LineIntersect(a, b, e3, e4).hit ||
			   LineIntersect(a, b, e4, e1).hit;
	}

	RectOverlap Physics::AIsNearB(const Entity& a, const Entity& b, const glm::vec2& maxDist)
	{
		ODirection dir = ODirection::NONE;
		glm::vec2 overlap = GetOverlap(a, b);
		glm::vec2 prevOverlap = GetPreviousOverlap(a, b);

		float dy = b.Get<TransformComponent>().pos.y - a.Get<TransformComponent>().pos.y;
		if (overlap.x > 0 && overlap.y > 0 && -maxDist.y < overlap.y && prevOverlap.y <= 0)
		{
			dir = dy > 0 ? ODirection::UP : ODirection::DOWN;
		}

		float dx = b.Get<TransformComponent>().pos.x - a.Get<TransformComponent>().pos.x;
		if (overlap.y > 0 && overlap.x > 0 && -maxDist.x < overlap.x && prevOverlap.x <= 0)
		{
			dir = dx > 0 ? ODirection::LEFT : ODirection::RIGHT;
		}

		return { dir, overlap };
	}

	glm::vec2 Physics::GetSpeedAB(const glm::vec2& posA, const glm::vec2& posB, float speed)
	{
		float theta = std::atan2(posB.y - posA.y, posB.x - posA.x);
		return glm::vec2(speed * std::cos(theta), speed * std::sin(theta));
	}
}
