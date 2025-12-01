#include "ScriptAPI/Physics2DAPI.h"

#include "ECS/Components/Components.h"
#include "ECS/Entity.h"

#include <glm/vec2.hpp>

namespace Luden
{
	namespace Physics2DAPI
	{
		void AddImpulse(Entity entity, glm::vec2 impulse)
		{
			if (!entity.Has<RigidBody2DComponent>()) 
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();
			b2Body_ApplyLinearImpulseToCenter(rb.RuntimeBodyId, b2Vec2(impulse.x, impulse.y), true);
		}

		void SetLinearVelocity(Entity entity, glm::vec2 velocity)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();
			b2Body_SetLinearVelocity(rb.RuntimeBodyId, b2Vec2(velocity.x, velocity.y));
		}

		glm::vec2 GetLinearVelocity(Entity entity)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return { 0.0f, 0.0f };

			auto& rb = entity.Get<RigidBody2DComponent>();
			b2Vec2 velocity = b2Body_GetLinearVelocity(rb.RuntimeBodyId);
			return { velocity.x, velocity.y };
		}
	}
}