#pragma once

#include "EngineAPI.h"
#include "glm/vec2.hpp"

namespace Luden
{
	class Entity;
	namespace Physics2DAPI
	{
		ENGINE_API void AddImpulse(Entity entity, glm::vec2 impulse);

		ENGINE_API void SetLinearVelocity(Entity entity, glm::vec2 velocity);
		ENGINE_API glm::vec2 GetLinearVelocity(Entity entity);
	}
}

