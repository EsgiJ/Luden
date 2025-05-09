#pragma once

#include <memory>
#include "Math/Vec2.h"
#include "ECS/Entity.h"
#include "EngineAPI.h"

namespace Luden
{
	struct Intersect
	{
		bool hit = false;
		Math::Vec2 position = { 0.0f, 0.0f };
	};

	class ENGINE_API Physics
	{
	public:
		static Math::Vec2 GetOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b);

		static Math::Vec2 GetPreviousOverlap(const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b);

		static bool IsInside(const Math::Vec2& pos, const std::shared_ptr<Entity>& entity);

		static Intersect LineIntersect(const Math::Vec2& a, const Math::Vec2& b, const Math::Vec2& c, const Math::Vec2& d);

		static bool EntityIntersect(const Math::Vec2& a, const Math::Vec2& b, const std::shared_ptr<Entity>& entity);
	};
}
