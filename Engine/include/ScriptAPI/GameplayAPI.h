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

	class ENGINE_API GameplayAPI
	{
	public:
		static void LoadScene(const std::string& sceneName);

		static Scene* GetCurrentScene();

		static void ReloadCurrentScene();

		static Entity SpawnEntity(const std::string& tag, const glm::vec3& location);


		static void DestroyEntity(Entity entity);

		static Entity FindEntityWithTag(const std::string& tag);

		static std::vector<Entity>& FindAllEntitiesWithTag(const std::string& tag);

		static std::vector<Entity> FindEntitiesInRadius(const glm::vec3& center, float radius);

		static float GetDeltaTime();

		static float GetGameTime();

		static float GetTimeScale();

		static void SetTimeScale(float scale);

		static void PlaySound2DAtLocation(const std::string& soundName, const glm::vec2& location);

		static void PlaySound2D(const std::string& soundName);

		static void QuitGame();
	};
}

