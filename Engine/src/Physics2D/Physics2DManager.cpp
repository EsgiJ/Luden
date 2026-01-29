#include "Physics2D/Physics2DManager.h"
#include "ECS/Components/Components.h"
#include "Debug/DebugManager.h"
#include "Scene/Scene.h"
#include "ECS/Entity.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "NativeScript/ScriptableEntity.h"

#include <glm/trigonometric.hpp>
#include <glm/common.hpp>
#include <box2d/box2d.h>
#include "Physics2D/CollisionContact.h"

namespace Luden
{
	void Physics2DManager::Init(Scene* scene, uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Scene = scene;
		m_ViewportWidth = viewportWidth;
		m_ViewportHeight = viewportHeight;

		if (m_Scene == nullptr)
			return;

		//TODO: Fix this hacky solution to update the entitiesToAdd
		m_Scene->GetEntityManager().Update(TimeStep(0.0f));

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = m_Gravity;
		m_PhysicsWorldId = b2CreateWorld(&worldDef);
		int bodyCount = 0;

		if (!b2World_IsValid(m_PhysicsWorldId))
			return;

		for (auto& entity : m_Scene->GetEntityManager().GetEntities())
		{
			if (entity.Has<RigidBody2DComponent>() && entity.Has<TransformComponent>())
			{
				auto& rb2d = entity.Get<RigidBody2DComponent>();
				auto& transformComponent = entity.Get<TransformComponent>();

				b2BodyDef bodyDef = b2DefaultBodyDef();

				if (rb2d.BodyType == RigidBody2DComponent::Type::Static)
					bodyDef.type = b2_staticBody;
				else if (rb2d.BodyType == RigidBody2DComponent::Type::Kinematic)
					bodyDef.type = b2_kinematicBody;
				else if (rb2d.BodyType == RigidBody2DComponent::Type::Dynamic)
					bodyDef.type = b2_dynamicBody;

				bodyDef.position = b2Vec2(
					transformComponent.Translation.x / m_PhysicsScale,
					(m_ViewportHeight - transformComponent.Translation.y) / m_PhysicsScale
				);
				bodyDef.rotation = b2MakeRot(glm::radians(transformComponent.angle));

				if (rb2d.FixedRotation)
				{
					bodyDef.motionLocks.angularZ = true;
				}

				bodyDef.linearDamping = rb2d.LinearDrag;
				bodyDef.angularDamping = rb2d.AngularDrag;
				bodyDef.gravityScale = rb2d.GravityScale;

				bodyDef.userData = (void*)(uintptr_t)entity.UUID();
				rb2d.RuntimeBodyId = b2CreateBody(m_PhysicsWorldId, &bodyDef);

				if (entity.Has<BoxCollider2DComponent>())
				{
					auto& bc2d = entity.Get<BoxCollider2DComponent>();
					b2BodyId bodyId = rb2d.RuntimeBodyId;

					float halfWidth = (bc2d.Size.x * transformComponent.Scale.x) / (2.0f * m_PhysicsScale);
					float halfHeight = (bc2d.Size.y * transformComponent.Scale.y) / (2.0f * m_PhysicsScale);

					b2Vec2 centerOffset = {
						bc2d.Offset.x / m_PhysicsScale,
						bc2d.Offset.y / m_PhysicsScale
					};

					b2Rot localRotation = b2MakeRot(0.0f);

					b2Polygon boxShape = b2MakeOffsetBox(
						(bc2d.Size.x * transformComponent.Scale.x) / (2.0f * m_PhysicsScale),
						(bc2d.Size.y * transformComponent.Scale.y) / (2.0f * m_PhysicsScale),
						centerOffset,
						localRotation
					);

					b2ShapeDef shapeDef = b2DefaultShapeDef();
					shapeDef.density = bc2d.Density;
					shapeDef.material.friction = bc2d.Friction;
					shapeDef.material.restitution = bc2d.Restitution;

					shapeDef.filter.categoryBits = bc2d.CategoryBits;
					shapeDef.filter.maskBits = bc2d.MaskBits;
					shapeDef.filter.groupIndex = bc2d.GroupIndex;

					shapeDef.enableContactEvents = true; 
					shapeDef.enableHitEvents = true;

					bc2d.RuntimeShapeId = b2CreatePolygonShape(bodyId, &shapeDef, &boxShape);
				}

				if (entity.Has<CircleCollider2DComponent>())
				{
					auto& cc2d = entity.Get<CircleCollider2DComponent>();
					b2BodyId bodyId = rb2d.RuntimeBodyId;

					float scale = glm::max(transformComponent.Scale.x, transformComponent.Scale.y);

					b2Circle circle =
					{
						{ cc2d.Offset.x / m_PhysicsScale, cc2d.Offset.y / m_PhysicsScale },
						(cc2d.Radius * scale) / m_PhysicsScale
					};

					b2ShapeDef shapeDef = b2DefaultShapeDef();

					shapeDef.density = cc2d.Density;
					shapeDef.material.friction = cc2d.Friction;
					shapeDef.material.restitution = cc2d.Restitution;

					shapeDef.filter.categoryBits = cc2d.CategoryBits;
					shapeDef.filter.maskBits = cc2d.MaskBits;
					shapeDef.filter.groupIndex = cc2d.GroupIndex;

					shapeDef.enableContactEvents = true;
					shapeDef.enableHitEvents = true;

					cc2d.RuntimeShapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
				}
			}
		}

		DebugManager::Instance().SetDebugDraw(m_PhysicsWorldId, m_PhysicsScale, m_ViewportHeight, m_ViewportWidth);
	}

	void Physics2DManager::Update(TimeStep ts)
	{
		if (!b2World_IsValid(m_PhysicsWorldId))
			return;

		if (m_Scene == nullptr)
			return;

		b2World_Step(m_PhysicsWorldId, static_cast<float>(ts), m_SubStepCount);

		for (auto& entity : m_Scene->GetEntityManager().GetEntities())
		{
			if (entity.Has<RigidBody2DComponent>() && entity.Has<TransformComponent>())
			{
				auto& rb2d = entity.Get<RigidBody2DComponent>();
				auto& transform = entity.Get<TransformComponent>();
				b2BodyId bodyId = rb2d.RuntimeBodyId;

				if (b2Body_IsValid(bodyId))
				{
					b2Vec2 position = b2Body_GetPosition(bodyId);
					b2Rot rotation = b2Body_GetRotation(bodyId);

					float angle = atan2f(rotation.s, rotation.c);

					transform.Translation.x = position.x * m_PhysicsScale;
					transform.Translation.y = m_ViewportHeight - (position.y * m_PhysicsScale);
					transform.angle = glm::degrees(angle);
				}
			}
		}

		ProcessContactEvents();
	}
	
	void Physics2DManager::Shutdown()
	{
		if (b2World_IsValid(m_PhysicsWorldId))
		{
			b2DestroyWorld(m_PhysicsWorldId);
			m_PhysicsWorldId = b2_nullWorldId;
		}

		if (m_Scene == nullptr)
			return;

		for (auto& e : m_Scene->GetEntityManager().GetEntities())
		{
			if (e.Has<RigidBody2DComponent>())
			{
				e.Get<RigidBody2DComponent>().RuntimeBodyId = b2_nullBodyId;
			}
		}
	}

	void Physics2DManager::RegisterEntity(Entity entity)
	{
		if (!b2World_IsValid(m_PhysicsWorldId))
			return;

		if (!entity.Has<RigidBody2DComponent>() || !entity.Has<TransformComponent>())
			return;

		auto& rb2d = entity.Get<RigidBody2DComponent>();
		auto& transformComponent = entity.Get<TransformComponent>();

		if (b2Body_IsValid(rb2d.RuntimeBodyId))
		{
			b2DestroyBody(rb2d.RuntimeBodyId);
			rb2d.RuntimeBodyId = b2_nullBodyId;
		}

		b2BodyDef bodyDef = b2DefaultBodyDef();

		if (rb2d.BodyType == RigidBody2DComponent::Type::Static)
			bodyDef.type = b2_staticBody;
		else if (rb2d.BodyType == RigidBody2DComponent::Type::Kinematic)
			bodyDef.type = b2_kinematicBody;
		else if (rb2d.BodyType == RigidBody2DComponent::Type::Dynamic)
			bodyDef.type = b2_dynamicBody;

		bodyDef.position = b2Vec2(
			transformComponent.Translation.x / m_PhysicsScale,
			(m_ViewportHeight - transformComponent.Translation.y) / m_PhysicsScale
		);
		bodyDef.rotation = b2MakeRot(glm::radians(transformComponent.angle));

		if (rb2d.FixedRotation)
		{
			bodyDef.motionLocks.angularZ = true;
		}

		bodyDef.linearDamping = rb2d.LinearDrag;
		bodyDef.angularDamping = rb2d.AngularDrag;
		bodyDef.gravityScale = rb2d.GravityScale;

		bodyDef.userData = (void*)(uintptr_t)entity.UUID();
		rb2d.RuntimeBodyId = b2CreateBody(m_PhysicsWorldId, &bodyDef);

		if (entity.Has<BoxCollider2DComponent>())
		{
			auto& bc2d = entity.Get<BoxCollider2DComponent>();
			b2BodyId bodyId = rb2d.RuntimeBodyId;

			b2Vec2 centerOffset = {
				bc2d.Offset.x / m_PhysicsScale,
				bc2d.Offset.y / m_PhysicsScale
			};

			b2Rot localRotation = b2MakeRot(0.0f);

			b2Polygon boxShape = b2MakeOffsetBox(
				(bc2d.Size.x * transformComponent.Scale.x) / (2.0f * m_PhysicsScale),
				(bc2d.Size.y * transformComponent.Scale.y) / (2.0f * m_PhysicsScale),
				centerOffset,
				localRotation
			);

			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.density = bc2d.Density;
			shapeDef.material.friction = bc2d.Friction;
			shapeDef.material.restitution = bc2d.Restitution;

			shapeDef.filter.categoryBits = bc2d.CategoryBits;
			shapeDef.filter.maskBits = bc2d.MaskBits;
			shapeDef.filter.groupIndex = bc2d.GroupIndex;

			shapeDef.enableContactEvents = true;
			shapeDef.enableHitEvents = true;

			bc2d.RuntimeShapeId = b2CreatePolygonShape(bodyId, &shapeDef, &boxShape);
		}

		if (entity.Has<CircleCollider2DComponent>())
		{
			auto& cc2d = entity.Get<CircleCollider2DComponent>();
			b2BodyId bodyId = rb2d.RuntimeBodyId;

			float scale = glm::max(transformComponent.Scale.x, transformComponent.Scale.y);

			b2Circle circle =
			{
				{ cc2d.Offset.x / m_PhysicsScale, cc2d.Offset.y / m_PhysicsScale },
				(cc2d.Radius * scale) / m_PhysicsScale
			};

			b2ShapeDef shapeDef = b2DefaultShapeDef();

			shapeDef.density = cc2d.Density;
			shapeDef.material.friction = cc2d.Friction;
			shapeDef.material.restitution = cc2d.Restitution;

			shapeDef.filter.categoryBits = cc2d.CategoryBits;
			shapeDef.filter.maskBits = cc2d.MaskBits;
			shapeDef.filter.groupIndex = cc2d.GroupIndex;

			shapeDef.enableContactEvents = true;
			shapeDef.enableHitEvents = true;

			cc2d.RuntimeShapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
		}
	}

	void Physics2DManager::UnregisterEntity(Entity entity)
	{
		if (!b2World_IsValid(m_PhysicsWorldId))
			return;

		if (!entity.Has<RigidBody2DComponent>())
			return;

		auto& rb2d = entity.Get<RigidBody2DComponent>();

		if (b2Body_IsValid(rb2d.RuntimeBodyId))
		{
			b2DestroyBody(rb2d.RuntimeBodyId);
			rb2d.RuntimeBodyId = b2_nullBodyId;
		}

		if (entity.Has<BoxCollider2DComponent>())
		{
			entity.Get<BoxCollider2DComponent>().RuntimeShapeId = b2_nullShapeId;
		}

		if (entity.Has<CircleCollider2DComponent>())
		{
			entity.Get<CircleCollider2DComponent>().RuntimeShapeId = b2_nullShapeId;
		}
	}

	void Physics2DManager::UpdateEntityPhysics(Entity entity)
	{
		UnregisterEntity(entity);
		RegisterEntity(entity);
	}

	void Physics2DManager::SetGravity(b2Vec2 gravity)
	{
		m_Gravity = gravity;

		if (b2World_IsValid(m_PhysicsWorldId))
		{
			b2World_SetGravity(m_PhysicsWorldId, gravity);
		}
	}

	void Physics2DManager::ProcessContactEvents()
	{
		b2ContactEvents events = b2World_GetContactEvents(m_PhysicsWorldId);
		
		// OnCollisionBegin logic
		for (int i = 0; i < events.beginCount; i++)
		{
			const b2ContactBeginTouchEvent& beginEvent = events.beginEvents[i];

			Entity entityA = m_Scene->FindEntityByShapeId(beginEvent.shapeIdA);
			Entity entityB = m_Scene->FindEntityByShapeId(beginEvent.shapeIdB);

			if (!entityA.IsValid() || !entityB.IsValid())
				continue;

			b2ContactData data = b2Contact_GetData(beginEvent.contactId);

			CollisionContact contactForA;
			contactForA.otherEntity = entityB;
			contactForA.isTouching = true;

			if (data.manifold.pointCount > 0)
			{
				contactForA.point = glm::vec2(
					data.manifold.points[0].anchorA.x * m_PhysicsScale,
					m_ViewportHeight - (data.manifold.points[0].anchorA.y * m_PhysicsScale)
				);
				contactForA.normal = glm::vec2(
					data.manifold.normal.x,
					-data.manifold.normal.y  // flip Y 
				);
				contactForA.normalImpulse = data.manifold.points[0].normalImpulse;
				contactForA.tangentImpulse = data.manifold.points[0].tangentImpulse;
			}
			if(entityA.Has<NativeScriptComponent>())
			{
				auto& nsc = entityA.Get<NativeScriptComponent>();
				if (nsc.Instance)
				{
					nsc.Instance->OnCollisionBegin(contactForA);
				}
			}

			CollisionContact contactForB;
			contactForB.otherEntity = entityA;
			contactForB.isTouching = true;

			if (data.manifold.pointCount > 0)
			{
				contactForB.point = glm::vec2(
					data.manifold.points[0].anchorB.x * m_PhysicsScale,
					m_ViewportHeight - (data.manifold.points[0].anchorB.y * m_PhysicsScale)
				);
				contactForB.normal = glm::vec2(
					-data.manifold.normal.x,  // Flip for opposite direction
					data.manifold.normal.y
				);
				contactForB.normalImpulse = data.manifold.points[0].normalImpulse;
				contactForB.tangentImpulse = data.manifold.points[0].tangentImpulse;
			}
			if (entityB.Has<NativeScriptComponent>())
			{
				auto& nsc = entityB.Get<NativeScriptComponent>();
				if (nsc.Instance)
				{
					nsc.Instance->OnCollisionBegin(contactForB);
				}
			}
		}

		// OnCollisionEnd logic
		for (int i = 0; i < events.beginCount; i++)
		{
			const b2ContactBeginTouchEvent& beginEvent = events.beginEvents[i];

			Entity entityA = m_Scene->FindEntityByShapeId(beginEvent.shapeIdA);
			Entity entityB = m_Scene->FindEntityByShapeId(beginEvent.shapeIdB);

			if (!entityA.IsValid() || !entityB.IsValid())
				continue;

			b2ContactData data = b2Contact_GetData(beginEvent.contactId);

			CollisionContact contactForA;
			contactForA.otherEntity = entityB;
			contactForA.isTouching = true;

			if (data.manifold.pointCount > 0)
			{
				contactForA.point = glm::vec2(
					data.manifold.points[0].anchorA.x * m_PhysicsScale,
					m_ViewportHeight - (data.manifold.points[0].anchorA.y * m_PhysicsScale)
				);
				contactForA.normal = glm::vec2(
					data.manifold.normal.x,
					-data.manifold.normal.y  // flip Y 
				);
				contactForA.normalImpulse = data.manifold.points[0].normalImpulse;
				contactForA.tangentImpulse = data.manifold.points[0].tangentImpulse;
			}
			if (entityA.Has<NativeScriptComponent>())
			{
				auto& nsc = entityA.Get<NativeScriptComponent>();
				if (nsc.Instance)
				{
					nsc.Instance->OnCollisionBegin(contactForA);
				}
			}

			CollisionContact contactForB;
			contactForB.otherEntity = entityA;
			contactForB.isTouching = true;

			if (data.manifold.pointCount > 0)
			{
				contactForB.point = glm::vec2(
					data.manifold.points[0].anchorB.x * m_PhysicsScale,
					m_ViewportHeight - (data.manifold.points[0].anchorB.y * m_PhysicsScale)
				);
				contactForB.normal = glm::vec2(
					-data.manifold.normal.x,  // Flip for opposite direction
					data.manifold.normal.y
				);
				contactForB.normalImpulse = data.manifold.points[0].normalImpulse;
				contactForB.tangentImpulse = data.manifold.points[0].tangentImpulse;
			}
			if (entityB.Has<NativeScriptComponent>())
			{
				auto& nsc = entityB.Get<NativeScriptComponent>();
				if (nsc.Instance)
				{
					nsc.Instance->OnCollisionBegin(contactForB);
				}
			}
		}

		// OnCollisionHit
		for (int i = 0; i < events.hitCount; i++)
		{
			const b2ContactHitEvent& hitEvent = events.hitEvents[i];

			Entity entityA = m_Scene->FindEntityByShapeId(hitEvent.shapeIdA);
			Entity entityB = m_Scene->FindEntityByShapeId(hitEvent.shapeIdB);

			if (!entityA.IsValid() || !entityB.IsValid())
				continue;

			CollisionContact contactForA;
			contactForA.otherEntity = entityB;
			contactForA.point = glm::vec2(
				hitEvent.point.x * m_PhysicsScale,
				m_ViewportHeight - (hitEvent.point.y * m_PhysicsScale)
			);
			contactForA.normal = glm::vec2(
				hitEvent.normal.x,
				-hitEvent.normal.y
			);
			contactForA.approachSpeed = hitEvent.approachSpeed;
			contactForA.isTouching = true;

			if (entityA.Has<NativeScriptComponent>())
			{
				auto& nsc = entityA.Get<NativeScriptComponent>();
				if (nsc.Instance)
				{
					nsc.Instance->OnCollisionHit(contactForA);
				}
			}

			CollisionContact contactForB;
			contactForB.otherEntity = entityA;
			contactForB.point = contactForA.point;
			contactForB.normal = glm::vec2(-hitEvent.normal.x, hitEvent.normal.y);
			contactForB.approachSpeed = hitEvent.approachSpeed;
			contactForB.isTouching = true;

			if (entityB.Has<NativeScriptComponent>())
			{
				auto& nsc = entityB.Get<NativeScriptComponent>();
				if (nsc.Instance)
				{
					nsc.Instance->OnCollisionHit(contactForB);
				}
			}
		}
	}
}