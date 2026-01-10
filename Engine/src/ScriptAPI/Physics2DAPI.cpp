#include "ScriptAPI/Physics2DAPI.h"
#include "ECS/Components/Components.h"
#include "ECS/Entity.h"
#include "Core/EngineContext.h"
#include "Scene/Scene.h"

#include <glm/vec2.hpp>
#include "glm/trigonometric.hpp"

namespace Luden
{
	namespace Physics2DAPI
	{
		b2WorldId GetPhysicsWorld()
		{
			return GEngine.GetActiveScene()->GetPhysicsWorldId();
		}

		void SetLinearVelocity(Entity entity, glm::vec2 velocity)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return;

			b2Body_SetLinearVelocity(rb.RuntimeBodyId, b2Vec2(velocity.x, velocity.y));
		}

		glm::vec2 GetLinearVelocity(Entity entity)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return { 0.0f, 0.0f };

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return {0.0f, 0.0f};

			b2Vec2 velocity = b2Body_GetLinearVelocity(rb.RuntimeBodyId);
			return { velocity.x, velocity.y };
		}

		void SetAngularVelocity(Entity entity, float velocity)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return;

			b2Body_SetAngularVelocity(rb.RuntimeBodyId, velocity);
		}

		float GetAngularVelocity(Entity entity)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return 0.0f;

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return 0.0f;

			float angularVelocity= b2Body_GetAngularVelocity(rb.RuntimeBodyId);
			return angularVelocity;
		}

		void AddImpulseToCenter(Entity entity, glm::vec2 impulse)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return;

			b2Body_ApplyLinearImpulseToCenter(rb.RuntimeBodyId, b2Vec2(impulse.x, impulse.y), true);
		}

		void AddForceToCenter(Entity entity, glm::vec2 force)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return;

			b2Body_ApplyForceToCenter(rb.RuntimeBodyId, b2Vec2(force.x, force.y), true);
		}

		void SetMass(Entity entity, float mass)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();
			rb.Mass = mass;
		}

		void SetFriction(Entity entity, float friction)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;
				
			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return;

			const int capacity = 8;
			b2ShapeId shapeIds[capacity];
			int count = b2Body_GetShapes(rb.RuntimeBodyId, shapeIds, capacity);

			if (count <= 0)
				return;

			for (int i = 0; i < count; i++)
			{
				if (b2Shape_IsValid(shapeIds[i]))
					b2Shape_SetFriction(shapeIds[i], friction);
			}
		}

		void SetTransform(Entity entity, glm::vec2 position, float angle)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return;

			Scene* scene = GEngine.GetActiveScene();
			if (!scene)
				return;

			auto& physicsManager = scene->GetPhysicsManager();
			float physicsScale = physicsManager.GetPhysicsScale();
			uint32_t viewportHeight = physicsManager.GetViewportHeight();

			b2Vec2 b2Position(
				position.x / physicsScale,
				(viewportHeight - position.y) / physicsScale
			);

			b2Rot rotation = b2MakeRot(glm::radians(angle));
			b2Body_SetTransform(rb.RuntimeBodyId, b2Position, rotation);
		}

		void SetGravityScale(Entity entity, float scale)
		{
			if (!entity.Has<RigidBody2DComponent>())
				return;

			auto& rb = entity.Get<RigidBody2DComponent>();

			if (!b2Body_IsValid(rb.RuntimeBodyId))
				return;

			b2Body_SetGravityScale(rb.RuntimeBodyId, scale);
		}

		RaycastHit RaycastClosest(glm::vec2 start, glm::vec2 end) {
			RaycastHit result;

			b2WorldId worldId = GEngine.GetActiveScene()->GetPhysicsWorldId();
			if (!b2World_IsValid(worldId)) {
				return result;
			}

			b2Vec2 origin{ start.x, start.y };
			b2Vec2 translation{ end.x - start.x, end.y - start.y };

			//TODO: Add channels to engine
			b2QueryFilter filter{};
			filter.categoryBits = 0xFFFF;
			filter.maskBits = 0xFFFF;

			b2RayResult hit = b2World_CastRayClosest(worldId, origin, translation, filter);

			// No hit
			if (!hit.hit) {
				return result;
			}

			result.hit = true;
			result.point = { hit.point.x, hit.point.y };
			result.normal = { hit.normal.x, hit.normal.y };
			result.fraction = hit.fraction; 

			if (b2Shape_IsValid(hit.shapeId)) {
				b2BodyId bodyId = b2Shape_GetBody(hit.shapeId);
				Entity hitEntity = GEngine.GetActiveScene()->FindEntityByBodyId(bodyId);

				if (hitEntity.IsValid()) {
					result.hitEntityId = hitEntity.UUID();
				}
			}

			return result;
		}

		std::vector<RaycastHit> Physics2DAPI::RaycastAll(glm::vec2 start, glm::vec2 end)
		{
			std::vector<RaycastHit> results;

			b2WorldId worldId = GEngine.GetActiveScene()->GetPhysicsWorldId();
			if (!b2World_IsValid(worldId))
				return results;

			b2Vec2 origin{ start.x, start.y };
			b2Vec2 translation{ end.x - start.x, end.y - start.y };

			//TODO: Add channels to engine
			b2QueryFilter filter{};
			filter.categoryBits = 0xFFFF;
			filter.maskBits = 0xFFFF;

			RaycastContext context;
			context.hits = &results;
			context.scene = GEngine.GetActiveScene();

			auto raycastCallback = [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* ctx) -> float
				{
					RaycastContext* context = static_cast<RaycastContext*>(ctx);

					RaycastHit hit;
					hit.hit = true;
					hit.point = { point.x, point.y };
					hit.normal = { normal.x, normal.y };
					hit.fraction = fraction;
					hit.shapeId = shapeId;

					if (b2Shape_IsValid(shapeId))
					{
						b2BodyId bodyId = b2Shape_GetBody(shapeId);
						Entity hitEntity = context->scene->FindEntityByBodyId(bodyId);

						if (hitEntity.IsValid())
						{
							hit.hitEntityId = hitEntity.UUID();
						}
					}

					context->hits->push_back(hit);

					// Continue and find all hits
					return 1.0f; 
				};

			b2World_CastRay(worldId, origin, translation, filter, raycastCallback, &context);

			// Sort by distance, idk why but fraction means distance apparently
			std::sort(results.begin(), results.end(),
				[](const RaycastHit& a, const RaycastHit& b) { return a.fraction < b.fraction; });

			return results;
		}
	}
}