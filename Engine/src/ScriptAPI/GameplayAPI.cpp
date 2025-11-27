#include "ScriptAPI/GameplayAPI.h"
#include "Core/EngineContext.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

namespace Luden
{
	void GameplayAPI::LoadScene(const std::string& sceneName)
	{
		//TODO:
	}

	Scene* GameplayAPI::GetCurrentScene()
	{
		return GEngine.GetActiveScene();
	}

	void GameplayAPI::ReloadCurrentScene()
	{
	}

	Entity GameplayAPI::SpawnEntity(const std::string& tag, const glm::vec3& location)
	{
		Entity entity = GEngine.GetActiveScene()->CreateEntity(tag);
		entity.Add<TransformComponent>().Translation = location;

		return entity;
	}

	void GameplayAPI::DestroyEntity(Entity entity)
	{
		entity.Destroy();
	}

	Entity GameplayAPI::FindEntityWithTag(const std::string& tag)
	{
		Entity entity = GEngine.GetActiveScene()->TryGetEntityWithTag(tag);

		return entity;
	}

	std::vector<Entity>& GameplayAPI::FindAllEntitiesWithTag(const std::string& tag)
	{
		return GEngine.GetActiveScene()->FindAllEntitiesWithTag(tag);
	}

	std::vector<Entity> GameplayAPI::FindEntitiesInRadius(const glm::vec3& center, float radius)
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

	float GameplayAPI::GetDeltaTime()
	{
		return GEngine.GetDeltaTime();
	}

	float GameplayAPI::GetGameTime()
	{
		return GEngine.GetGameTime();
	}

	float GameplayAPI::GetTimeScale()
	{
		return GEngine.GetTimeScale();
	}

}


