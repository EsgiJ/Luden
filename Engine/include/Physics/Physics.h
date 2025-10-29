#pragma once

#include <memory>

#include "ECS/Entity.h"
#include "EngineAPI.h"
#include <glm/vec2.hpp>

namespace Luden
{
	struct ENGINE_API Intersect
	{
		bool hit = false;
		glm::vec2 position = { 0.0f, 0.0f };
	};

	enum struct ODirection 
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NONE
	};

	struct ENGINE_API RectOverlap 
	{
		ODirection m_Direction = ODirection::NONE;
		glm::vec2 m_Overlap = { 0.0f, 0.0f };
	};

	class ENGINE_API Physics
	{
	public:
		static glm::vec2 GetOverlap(const Entity& a, const Entity& b);

		static glm::vec2 GetPreviousOverlap(const Entity& a, const Entity& b);

		static bool IsInside(const glm::vec2& pos, const Entity& entity);

		static Intersect LineIntersect(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d);

		static bool EntityIntersect(const glm::vec2& a, const glm::vec2& b, const Entity& entity);

		static RectOverlap AIsNearB(const Entity& a, const Entity& b, const glm::vec2& maxDist);

		static glm::vec2 GetSpeedAB(const glm::vec2& posA, const glm::vec2& posB, float speed);
	};
}
