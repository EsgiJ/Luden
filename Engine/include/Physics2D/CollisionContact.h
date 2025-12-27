#pragma once

#include "EngineAPI.h"
#include "ECS/Entity.h"

#include <glm/vec2.hpp>

namespace Luden
{
	struct ENGINE_API CollisionContact
	{
		Entity otherEntity;
		glm::vec2 point = { 0.0f, 0.0f };
		glm::vec2 normal = { 0.0f, 0.0f };
		float normalImpulse = 0.0f;
		float tangentImpulse = 0.0f;
		float approachSpeed = 0.0f;  
		bool isTouching = false;
	};
}