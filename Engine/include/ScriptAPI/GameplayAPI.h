#pragma once

#include "EngineAPI.h"

#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Luden
{
	class Scene;
	class Entity;
	namespace GameplayAPI
	{
		 ENGINE_API void LoadScene(const std::string& sceneName);

		 ENGINE_API Scene* GetCurrentScene();

		 ENGINE_API void ReloadCurrentScene();

		 ENGINE_API Entity SpawnEntity(const std::string& tag, const glm::vec3& location);

		 ENGINE_API void DestroyEntity(Entity entity);

		 ENGINE_API Entity FindEntityWithTag(const std::string& tag);

		 ENGINE_API std::vector<Entity>& FindAllEntitiesWithTag(const std::string& tag);

		 ENGINE_API std::vector<Entity> FindEntitiesInRadius(const glm::vec3& center, float radius);

		 ENGINE_API float GetDeltaTime();

		 ENGINE_API float GetGameTime();

		 ENGINE_API float GetTimeScale();

		 ENGINE_API void SetTimeScale(float scale);

		 ENGINE_API void QuitGame();
	}
}

