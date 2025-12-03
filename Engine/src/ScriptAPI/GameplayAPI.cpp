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

namespace Luden
{
	namespace GameplayAPI
	{
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

		Entity SpawnEntity(const std::string& tag, const glm::vec3& location)
		{
			Entity entity = GEngine.GetActiveScene()->CreateEntity(tag);
			entity.Add<TransformComponent>().Translation = location;

			return entity;
		}

		void DestroyEntity(Entity entity)
		{
			entity.Destroy();
		}

		Entity FindEntityWithTag(const std::string& tag)
		{
			Entity entity = GEngine.GetActiveScene()->TryGetEntityWithTag(tag);

			return entity;
		}

		std::vector<Entity>& FindAllEntitiesWithTag(const std::string& tag)
		{
			return GEngine.GetActiveScene()->FindAllEntitiesWithTag(tag);
		}

		std::vector<Entity> FindEntitiesInRadius(const glm::vec3& center, float radius)
		{
			std::vector<Entity> result;

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
	}
}


