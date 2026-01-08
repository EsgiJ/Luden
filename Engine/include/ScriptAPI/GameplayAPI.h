#pragma once

#include "EngineAPI.h"
#include "Luden.h"

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
		ENGINE_API Entity SpawnPrefab(PrefabRef prefab, const glm::vec3& location);
		ENGINE_API Entity SpawnPrefabAsChild(PrefabRef prefab, Entity parent, const glm::vec3& localPosition);

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

		 ENGINE_API bool IsGrounded(Entity entity);
		 ENGINE_API void LookAt(Entity source, Entity target);
		 
		 ENGINE_API void QuitGame();

		 ENGINE_API float RandomFloat(float min = 0.0f, float max = 1.0f);
		 ENGINE_API int RandomInt(int min, int max);
		 ENGINE_API bool RandomBool(float probability = 0.5f);
	}
}

