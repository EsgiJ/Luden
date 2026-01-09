#include "ScriptAPI/GameplayAPI.h"
#include "Core/EngineContext.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "ScriptAPI/Physics2DAPI.h"

#include <cmath>


#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <random>

namespace Luden
{
	namespace GameplayAPI
	{
		Entity SpawnPrefab(PrefabRef prefab, const glm::vec3& location)
		{
			if (!prefab)
			{
				std::cerr << "[GameplayAPI] Invalid prefab reference!" << std::endl;
				return {};
			}

			Scene* scene = GEngine.GetActiveScene();
			if (!scene)
			{
				std::cerr << "[GameplayAPI] No active scene!" << std::endl;
				return {};
			}

			return scene->Instantiate(prefab, &location, nullptr, nullptr);
		}

		Entity SpawnPrefabAsChild(PrefabRef prefab, Entity parent, const glm::vec3& localPosition)
		{
			if (!prefab)
			{
				std::cerr << "[GameplayAPI] Invalid prefab reference!" << std::endl;
				return {};
			}

			if (!parent.IsValid())
			{
				std::cerr << "[GameplayAPI] Invalid parent entity!" << std::endl;
				return {};
			}

			Scene* scene = GEngine.GetActiveScene();
			if (!scene)
			{
				std::cerr << "[GameplayAPI] No active scene!" << std::endl;
				return {};
			}

			return scene->InstantiateChild(prefab, parent, &localPosition, nullptr, nullptr);
		}

		Entity SpawnEntity(const String& tag, const Vec3& location)
		{
			Scene* scene = GetCurrentScene();
			if (!scene)
				return {};

			Entity entity = scene->CreateEntity(tag);
			entity.Get<TransformComponent>().Translation = location;

			if (entity.Has<RigidBody2DComponent>())
			{
				scene->GetPhysicsManager().RegisterEntity(entity);
			}

			return entity;
		}

		Entity DuplicateEntity(Entity entity)
		{
			if (!entity.IsValid())
				return {};

			Scene* scene = GetCurrentScene();
			if (!scene)
				return {};

			return scene->DuplicateEntity(entity);
		}

		void LoadScene(const std::string& sceneName)
		{
			//TODO:
		}

		Scene* GetCurrentScene()
		{
			return GEngine.GetActiveScene();
		}

		void ReloadCurrentScene()
		{
		}

		void QuitGame()
		{
			return void();
		}

		Entity FindEntityWithTag(const String& tag)
		{
			Entity entity = GEngine.GetActiveScene()->TryGetEntityWithTag(tag);

			return entity;
		}

		Vector<Entity>& FindAllEntitiesWithTag(const String& tag)
		{
			return GEngine.GetActiveScene()->FindAllEntitiesWithTag(tag);
		}

		Vector<Entity> FindEntitiesInRadius(const Vec3& center, float radius)
		{
			Vector<Entity> result;

			float radiusSquared = radius * radius;

			auto& entities = GEngine.GetActiveScene()->GetEntityManager().GetEntities();

			for (auto& entity : entities)
			{
				if (entity.Has<TransformComponent>())
				{
					auto& translation = entity.Get<TransformComponent>().Translation;

					// glm::distance returns length(a -b), so we have to use radiusSquared
					if (glm::distance(center, translation) <= radiusSquared)
						result.push_back(entity);
				}
			}

			return result;
		}

		Entity FindClosestEntity(const Vec3& position, const String& tag)
		{
			auto& entities = FindAllEntitiesWithTag(tag);

			Entity closest;
			float minDistSq = FLT_MAX;

			for (auto& entity : entities)
			{
				Vec3 entityPos = GetPosition(entity);
				Vec3 diff = entityPos - position;
				float distSq = glm::dot(diff, diff);

				if (distSq < minDistSq)
				{
					minDistSq = distSq;
					closest = entity;
				}
			}

			return closest;
		}

		void SetPosition(Entity entity, const Vec3& position)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			entity.Get<TransformComponent>().Translation = position;
		}

		Vec3 GetPosition(Entity entity)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return Vec3(0.0f);

			return entity.Get<TransformComponent>().Translation;
		}

		void Move(Entity entity, const Vec3& offset)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			entity.Get<TransformComponent>().Translation += offset;
		}

		void MoveTowards(Entity entity, const Vec3& target, float maxDistance)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			auto& transform = entity.Get<TransformComponent>();
			Vec3 direction = glm::normalize(target - transform.Translation);
			Vec3 offset = direction * maxDistance;

			float dist = glm::distance(transform.Translation, target);
			if (dist < maxDistance)
			{
				transform.Translation = target;
			}
			else
			{
				transform.Translation += offset;
			}
		}

		void SetRotation(Entity entity, float degrees)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			entity.Get<TransformComponent>().angle = degrees;
		}

		float GetRotation(Entity entity)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return 0.0f;

			return entity.Get<TransformComponent>().angle;
		}

		void Rotate(Entity entity, float degrees)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			entity.Get<TransformComponent>().angle += degrees;
		}

		void DestroyEntity(Entity entity)
		{
			Scene* scene = GetCurrentScene();
			if (scene && entity.Has<RigidBody2DComponent>())
			{
				scene->GetPhysicsManager().UnregisterEntity(entity);
			}

			entity.Destroy();
		}

		float GetDeltaTime()
		{
			return GEngine.GetDeltaTime();
		}

		float GetGameTime()
		{
			return GEngine.GetGameTime();
		}

		float GetTimeScale()
		{
			return GEngine.GetTimeScale();
		}

		bool IsGrounded(Entity entity)
		{
			if (!entity.Has<TransformComponent>())
				return false;
			
			const float distCheck = 2.0f;

			glm::vec2 start = entity.Get<TransformComponent>().Translation;
			glm::vec2 end = start + glm::vec2(0.0f, distCheck);

			auto hit = Physics2DAPI::RaycastClosest(start, end);

			return hit.hit;
		}

		bool IsInRange(Entity source, Entity target, float range)
		{
			return DistanceSquared(source, target) <= (range * range);
		}

		bool IsInRange(const Vec3& source, const Vec3& target, float range)
		{
			Vec3 diff = target - source;
			return glm::dot(diff, diff) <= (range * range);
		}

		void LookAt(Entity source, Entity target)
		{
			if (!source.Has<TransformComponent>() || !target.Has<TransformComponent>())
				return;

			auto& sourceTransform = source.Get<TransformComponent>();
			glm::vec2 sourcePos = sourceTransform.Translation;
			glm::vec2 targetPos = target.Get<TransformComponent>().Translation;

			glm::vec2 dir = targetPos - sourcePos;

			float angleRad = std::atan2(dir.x, dir.y);
			float angleDegree = glm::degrees(angleRad);

			sourceTransform.angle = angleDegree;
		}

		void LookAtPosition(Entity source, const Vec3& target)
		{
			if (!source.IsValid() || !source.Has<TransformComponent>())
				return;

			auto& transform = source.Get<TransformComponent>();
			Vec3 direction = target - transform.Translation;

			float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
			transform.angle = angle;
		}

		float Distance(Entity a, Entity b)
		{
			return glm::distance(GetPosition(a), GetPosition(b));
		}

		float Distance(const Vec3& a, const Vec3& b)
		{
			return glm::distance(a, b);
		}

		float DistanceSquared(Entity a, Entity b)
		{
			Vec3 diff = GetPosition(a) - GetPosition(b);
			return glm::dot(diff, diff);
		}

		Vec2 Direction2D(Entity from, Entity to)
		{
			Vec3 dir = GetPosition(to) - GetPosition(from);
			return glm::normalize(Vec2(dir.x, dir.y));
		}

		Vec2 Direction2D(const Vec3& from, const Vec3& to)
		{
			Vec3 dir = to - from;
			return glm::normalize(Vec2(dir.x, dir.y));
		}

		Vec3 Direction(Entity from, Entity to)
		{
			return glm::normalize(GetPosition(to) - GetPosition(from));
		}

		Vec3 Direction(const Vec3& from, const Vec3& to)
		{
			return glm::normalize(to - from);
		}

		void SetScale(Entity entity, const Vec3& scale)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			entity.Get<TransformComponent>().Scale = scale;
		}

		void SetScale(Entity entity, float uniformScale)
		{
			SetScale(entity, Vec3(uniformScale, uniformScale, uniformScale));
		}

		Vec3 GetScale(Entity entity)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return Vec3(1.0f);

			return entity.Get<TransformComponent>().Scale;
		}

		void Scale(Entity entity, const Vec3& scale)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			auto& transform = entity.Get<TransformComponent>();
			transform.Scale.x *= scale.x;
			transform.Scale.y *= scale.y;
			transform.Scale.z *= scale.z;
		}

		Vec2 GetEntitySize(Entity entity)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return Vec2(0.0f, 0.0f);

			Vec2 size = GetEntitySizeUnscaled(entity);
			Vec3 scale = entity.Get<TransformComponent>().Scale;

			return Vec2(size.x * scale.x, size.y * scale.y);
		}

		Vec2 GetEntitySizeUnscaled(Entity entity)
		{
			if (!entity.IsValid())
				return Vec2(0.0f, 0.0f);

			if (entity.Has<SpriteAnimatorComponent>())
			{
				auto& animator = entity.Get<SpriteAnimatorComponent>();

				if (animator.animationHandles.empty())
					return Vec2(0.0f, 0.0f);

				if (animator.currentAnimationIndex >= animator.animationHandles.size())
					return Vec2(0.0f, 0.0f);

				auto animation = ResourceManager::GetResource<Animation>(
					animator.animationHandles[animator.currentAnimationIndex]
				);

				if (!animation || animation->GetFrameCount() == 0)
					return Vec2(0.0f, 0.0f);

				uint32_t frameIndex = animator.currentFrame;
				if (frameIndex >= animation->GetFrameCount())
					frameIndex = 0;

				const auto& frame = animation->GetFrame(frameIndex);
				auto sprite = ResourceManager::GetResource<Sprite>(frame.spriteHandle);

				if (!sprite)
					return Vec2(0.0f, 0.0f);

				sf::Vector2u texSize = sprite->GetSize();
				return Vec2((float)texSize.x, (float)texSize.y);
			}

			if (entity.Has<SpriteRendererComponent>())
			{
				auto& spriteComp = entity.Get<SpriteRendererComponent>();

				if (spriteComp.spriteHandle == 0)
					return Vec2(0.0f, 0.0f);

				auto sprite = ResourceManager::GetResource<Sprite>(spriteComp.spriteHandle);

				if (!sprite)
					return Vec2(0.0f, 0.0f);

				sf::Vector2u texSize = sprite->GetSize();
				return Vec2((float)texSize.x, (float)texSize.y);
			}

			return Vec2(0.0f, 0.0f);
		}
	}
}


