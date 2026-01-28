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
		ENGINE_API void ParentEntity(Entity& entity, Entity& parent);
		ENGINE_API void UnparentEntity(Entity& entity);
		ENGINE_API Entity DuplicateEntity(Entity entity);
		ENGINE_API void DestroyEntity(Entity entity);

		ENGINE_API void ChangeScene(const std::string& sceneName);
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
		ENGINE_API float DistanceSquared(const Vec3& a, const Vec3& b);
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
		ENGINE_API void SetWorldGravity(Vec2 gravity);


		ENGINE_API bool IsGrounded(Entity entity);
		ENGINE_API bool IsInRange(Entity source, Entity target, float range);
		ENGINE_API bool IsInRange(const Vec3& source, const Vec3& target, float range);

		ENGINE_API Vec2 GetEntitySize(Entity entity);
		ENGINE_API Vec2 GetEntitySizeUnscaled(Entity entity);

		ENGINE_API Entity GetMainCameraEntity();
		ENGINE_API void ShakeCamera(const CameraShakeParams& params, float scale = 1.0f);
		ENGINE_API void StopCameraShake();
		ENGINE_API Vec2 GetCameraPosition();
		ENGINE_API void SetCameraPosition(const Vec2& position);
		ENGINE_API float GetCameraZoom();
		ENGINE_API void SetCameraZoom(float zoom);

		ENGINE_API Vec2 GetMousePosition();              
		ENGINE_API Entity GetEntityUnderMouse();

		ENGINE_API bool IsOnScreen(Entity entity);
		ENGINE_API bool IsOnScreen(const Vec3& worldPos);
		ENGINE_API Vec2 GetScreenSize();
		ENGINE_API Vec2 GetWorldBounds();

		ENGINE_API bool CheckCircleOverlap(const Vec3& posA, float radiusA, const Vec3& posB, float radiusB);
		ENGINE_API bool CheckAABBOverlap(const Vec3& posA, const Vec2& sizeA, const Vec3& posB, const Vec2& sizeB);
		ENGINE_API bool IsPointInCircle(const Vec2& point, const Vec3& center, float radius);
		ENGINE_API bool IsPointInRect(const Vec2& point, const Vec3& rectPos, const Vec2& rectSize);

		template<typename T>
		T* GetScript(Entity entity)
		{
			if (!entity.IsValid() || !entity.Has<NativeScriptComponent>())
				return nullptr;

			auto& nsc = entity.Get<NativeScriptComponent>();
			if (!nsc.Instance)
				return nullptr;

			return dynamic_cast<T*>(nsc.Instance);
		}

		template<typename T>
		T* FindScript(const std::string& tag)
		{
			Entity entity = FindEntityWithTag(tag);
			if (!entity.IsValid())
				return nullptr;

			return GetScript<T>(entity);
		}

		template<typename T>
		std::vector<T*> FindAllScripts(const std::string& tag)
		{
			std::vector<T*> scripts;

			auto entities = FindAllEntitiesWithTag(tag);

			for (auto& entity : entities)
			{
				T* script = GetScript<T>(entity);
				if (script)
					scripts.push_back(script);
			}

			return scripts;
		}
	}
}

