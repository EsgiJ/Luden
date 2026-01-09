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
		ENGINE_API Entity SpawnEntity(const String& tag, const Vec3& location);
		ENGINE_API Entity DuplicateEntity(Entity entity);
		ENGINE_API void DestroyEntity(Entity entity);

		ENGINE_API void LoadScene(const std::string& sceneName);
		ENGINE_API Scene* GetCurrentScene();
		ENGINE_API void ReloadCurrentScene();
		ENGINE_API void QuitGame();

		ENGINE_API Entity FindEntityWithTag(const String& tag);
		ENGINE_API Vector<Entity>& FindAllEntitiesWithTag(const String& tag);
		ENGINE_API Vector<Entity> FindEntitiesInRadius(const Vec3& center, float radius);
		ENGINE_API Entity FindClosestEntity(const Vec3& position, const String& tag);

		ENGINE_API void SetPosition(Entity entity, const Vec3& position);
		ENGINE_API Vec3 GetPosition(Entity entity);
		ENGINE_API void Move(Entity entity, const Vec3& offset);
		ENGINE_API void MoveTowards(Entity entity, const Vec3& target, float maxDistance);

		ENGINE_API void SetRotation(Entity entity, float degrees);
		ENGINE_API float GetRotation(Entity entity);
		ENGINE_API void Rotate(Entity entity, float degrees);
		ENGINE_API void LookAt(Entity source, Entity target);
		ENGINE_API void LookAtPosition(Entity source, const Vec3& target);

		ENGINE_API float Distance(Entity a, Entity b);
		ENGINE_API float Distance(const Vec3& a, const Vec3& b);
		ENGINE_API float DistanceSquared(Entity a, Entity b);
		ENGINE_API Vec2 Direction2D(Entity from, Entity to);
		ENGINE_API Vec2 Direction2D(const Vec3& from, const Vec3& to);
		ENGINE_API Vec3 Direction(Entity from, Entity to);
		ENGINE_API Vec3 Direction(const Vec3& from, const Vec3& to);

		ENGINE_API void SetScale(Entity entity, const Vec3& scale);
		ENGINE_API void SetScale(Entity entity, float uniformScale);
		ENGINE_API Vec3 GetScale(Entity entity);
		ENGINE_API void Scale(Entity entity, const Vec3& scale);

		ENGINE_API float GetDeltaTime();
		ENGINE_API float GetGameTime();
		ENGINE_API float GetTimeScale();
		ENGINE_API void SetTimeScale(float scale);

		ENGINE_API bool IsGrounded(Entity entity);
		ENGINE_API bool IsInRange(Entity source, Entity target, float range);
		ENGINE_API bool IsInRange(const Vec3& source, const Vec3& target, float range);
	}
}

