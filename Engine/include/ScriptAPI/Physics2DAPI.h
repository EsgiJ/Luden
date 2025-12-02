#pragma once

#include "EngineAPI.h"
#include "ECS/Entity.h"

#include <glm/vec2.hpp>
#include <vector>

namespace Luden
{
	class Entity;
	namespace Physics2DAPI
	{
		struct ENGINE_API RaycastHit {
			bool hit = false;
			UUID hitEntityId;
			glm::vec2 point;
			glm::vec2 normal;
			float fraction = 0.0f; 
			b2ShapeId shapeId = b2_nullShapeId;
		};

		struct ENGINE_API RaycastContext
		{
			std::vector<RaycastHit>* hits;
			Scene* scene;
			float physicsScale;
			float viewportHeight;
		};

		ENGINE_API b2WorldId GetPhysicsWorldId();

		ENGINE_API void SetLinearVelocity(Entity entity, glm::vec2 velocity);
		ENGINE_API glm::vec2 GetLinearVelocity(Entity entity);
		
		ENGINE_API void SetAngularVelocity(Entity entity, float velocity);
		ENGINE_API float GetAngularVelocity(Entity entity);

		ENGINE_API void AddImpulseToCenter(Entity entity, glm::vec2 impulse);
		ENGINE_API void AddForceToCenter(Entity entity, glm::vec2 force);

		ENGINE_API void SetMass(Entity entity, float mass);
		ENGINE_API void SetFriction(Entity entity, float friction);
		ENGINE_API void SetTransform(Entity entity, glm::vec2 position, float angle);
		ENGINE_API void SetGravityScale(Entity entity, float scale);

		ENGINE_API RaycastHit RaycastClosest(glm::vec2 start, glm::vec2 end);
		ENGINE_API std::vector<RaycastHit> RaycastAll(glm::vec2 start, glm::vec2 end);
	}
}

