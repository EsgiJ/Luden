#include "ScriptAPI/GameplayAPI.h"
#include "Core/EngineContext.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "ScriptAPI/MathAPI.h"
#include "Input/InputManager.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include <cmath>
#include <iostream>
#include <random>
#include <SFML/Graphics/RenderWindow.hpp>


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

		void ParentEntity(Entity& entity, Entity& parent)
		{
			Scene* scene = GetCurrentScene();
			if (!scene)
				return;

			scene->ParentEntity(entity, parent);
		}

		void UnparentEntity(Entity& entity)
		{
			Scene* scene = GetCurrentScene();
			if (!scene)
				return;

			scene->UnparentEntity(entity);
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

		void ChangeScene(const std::string& sceneName)
		{
			Scene* currentScene = GetCurrentScene();
			if (!currentScene)
				return;

			auto resourceManager = Project::GetEditorResourceManager();
			if (!resourceManager)
				return;

			auto allScenes = resourceManager->GetAllResourcesWithType(ResourceType::Scene);

			for (auto sceneHandle : allScenes)
			{
				
				auto metadata = resourceManager->GetMetadata(sceneHandle);

				if (metadata.FilePath.filename().stem() == sceneName)
				{
					auto newScene = ResourceManager::GetResource<Scene>(sceneHandle);

					if (newScene)
					{
						currentScene->OnRuntimeStop();

						GEngine.SetActiveScene(newScene.get());

						newScene->OnRuntimeStart();

						return;
					}
				}
			}
		}

		Scene* GetCurrentScene()
		{
			return GEngine.GetActiveScene();
		}

		void ReloadCurrentScene()
		{
			Scene* currentScene = GetCurrentScene();
			if (!currentScene)
				return;

			std::string sceneName = currentScene->GetName();
			ChangeScene(sceneName);
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

			if (entity.Has<RigidBody2DComponent>())
			{
				Scene* scene = GetCurrentScene();
				if (scene)
				{
					Physics2DAPI::SetTransform(entity, Vec2(position.x, position.y), 0.0f);
				}
			}
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

			if (entity.Has<RigidBody2DComponent>())
			{
				auto& transform = entity.Get<TransformComponent>();
				Physics2DAPI::SetTransform(
					entity,
					Vec2(transform.Translation.x, transform.Translation.y),
					transform.angle
				);
			}
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

			if (entity.Has<RigidBody2DComponent>())
			{
				Physics2DAPI::SetTransform(
					entity,
					Vec2(transform.Translation.x, transform.Translation.y),
					transform.angle
				);
			}
		}

		void SetRotation(Entity entity, float degrees)
		{
			if (!entity.IsValid() || !entity.Has<TransformComponent>())
				return;

			entity.Get<TransformComponent>().angle = degrees;

			if (entity.Has<RigidBody2DComponent>())
			{
				auto& transform = entity.Get<TransformComponent>();
				Physics2DAPI::SetTransform(
					entity,
					Vec2(transform.Translation.x, transform.Translation.y),
					degrees
				);
			}
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

			if (entity.Has<RigidBody2DComponent>())
			{
				auto& transform = entity.Get<TransformComponent>();
				Physics2DAPI::SetTransform(
					entity,
					Vec2(transform.Translation.x, transform.Translation.y),
					transform.angle
				);
			}
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

		void SetWorldGravity(Vec2 gravity)
		{
			Scene* scene = GetCurrentScene();
			if (!scene)
				return;

			scene->SetWorldGravity(b2Vec2(gravity.x, gravity.y));
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

			float angleRad = std::atan2(dir.y, dir.x);
			float angleDegree = glm::degrees(angleRad);

			sourceTransform.angle = angleDegree;

			if (source.Has<RigidBody2DComponent>())
			{
				Physics2DAPI::SetTransform(
					source,
					Vec2(sourcePos.x, sourcePos.y),
					angleDegree
				);
			}
		}

		void LookAtPosition(Entity source, const Vec3& target)
		{
			if (!source.IsValid() || !source.Has<TransformComponent>())
				return;

			auto& transform = source.Get<TransformComponent>();
			Vec3 direction = target - transform.Translation;

			float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;


			angle += 90.0f;  

			transform.angle = angle;

			if (source.Has<RigidBody2DComponent>())
			{
				Physics2DAPI::SetTransform(
					source,
					Vec2(transform.Translation.x, transform.Translation.y),
					angle
				);
			}
		}

		float Distance(Entity a, Entity b)
		{
			return glm::distance(GetPosition(a), GetPosition(b));
		}

		float Distance(const Vec3& a, const Vec3& b)
		{
			return glm::distance(a, b);
		}

		float DistanceSquared(const Vec3& a, const Vec3& b)
		{
			Vec3 diff = a - b;
			return glm::dot(diff, diff);
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


			if (entity.Has<RigidBody2DComponent>())
			{
				Scene* scene = GetCurrentScene();
				if (scene)
				{
					scene->GetPhysicsManager().UpdateEntityPhysics(entity);
				}
			}
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

				size_t frameIndex = animator.currentFrame;
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

		Entity GetMainCameraEntity()
		{
			Scene* scene = GEngine.GetActiveScene();
			if (!scene)
				return {};

			return scene->GetMainCameraEntity();
		}

		void ShakeCamera(const CameraShakeParams& params, float scale)
		{
			Entity cameraEntity = GetMainCameraEntity();

			if (!cameraEntity.IsValid())
				return;

			if (cameraEntity.Has<Camera2DComponent>())
			{
				auto& camera = cameraEntity.Get<Camera2DComponent>();
				camera.Camera.Shake(params, scale);
			}
		}

		void StopCameraShake()
		{
			Entity cameraEntity = GetMainCameraEntity();

			if (cameraEntity.Has<Camera2DComponent>())
			{
				auto& camera = cameraEntity.Get<Camera2DComponent>();
				camera.Camera.StopShake();
			}
		}

		Vec2 GetMousePosition()
		{
			sf::RenderWindow* window = GEngine.GetWindow();
			if (!window)
				return { 0.0f, 0.0f };

			sf::Vector2i windowMousePos = sf::Mouse::getPosition(*window);

			glm::vec2 viewportPos = GEngine.GetViewportPosition();
			glm::vec2 viewportSize = GEngine.GetViewportSize();

			float viewportMouseX = (float)windowMousePos.x - viewportPos.x;
			float viewportMouseY = (float)windowMousePos.y - viewportPos.y;

			if (viewportMouseX < 0.0f || viewportMouseX > viewportSize.x ||
				viewportMouseY < 0.0f || viewportMouseY > viewportSize.y)
			{
				return { -1.0f, -1.0f };
			}

			Scene* scene = GetCurrentScene();
			if (!scene)
				return { viewportMouseX, viewportMouseY };

			Entity cameraEntity = scene->GetMainCameraEntity();
			if (!cameraEntity.IsValid() || !cameraEntity.Has<Camera2DComponent>())
				return { viewportMouseX, viewportMouseY };

			sf::RenderTexture* renderTexture = GEngine.GetRenderTexture();
			if (!renderTexture)
				return { viewportMouseX, viewportMouseY };

			auto& camera = cameraEntity.Get<Camera2DComponent>();
			const sf::View& view = camera.Camera.GetView();

			sf::Vector2u textureSize = renderTexture->getSize();

			float scaleX = (float)textureSize.x / viewportSize.x;
			float scaleY = (float)textureSize.y / viewportSize.y;


			float textureMouseX = viewportMouseX * scaleX;
			float textureMouseY = viewportMouseY * scaleY;


			sf::Vector2i texturePixel((int)textureMouseX, (int)textureMouseY);
			sf::Vector2f worldPos = renderTexture->mapPixelToCoords(texturePixel, view);

			return { worldPos.x, worldPos.y };
		}


		Entity GetEntityUnderMouse()
		{
			Vec2 mouseWorld = GetMousePosition();
			Scene* scene = GetCurrentScene();
			if (!scene)
				return {};

			for (auto& entity : scene->GetEntityManager().GetEntities())
			{
				if (!entity.Has<TransformComponent>())
					continue;

				Vec3 pos = entity.Get<TransformComponent>().Translation;
				Vec2 size = GetEntitySize(entity);

				if (IsPointInRect(mouseWorld, pos, size))
					return entity;
			}

			return {};
		}

		bool IsOnScreen(Entity entity)
		{
			if (!entity.IsValid())
				return false;

			Vec3 pos = GetPosition(entity);
			return IsOnScreen(pos);
		}

		bool IsOnScreen(const Vec3& worldPos)
		{
			Scene* scene = GetCurrentScene();
			if (!scene)
				return false;

			Entity cameraEntity = scene->GetMainCameraEntity();
			if (!cameraEntity.IsValid() || !cameraEntity.Has<Camera2DComponent>())
				return false;

			sf::RenderTexture* renderTexture = GEngine.GetRenderTexture();
			if (!renderTexture)
				return false;

			auto& camera = cameraEntity.Get<Camera2DComponent>();
			const sf::View& view = camera.Camera.GetView();

			sf::Vector2f viewCenter = view.getCenter();
			sf::Vector2f viewSize = view.getSize();

			float halfWidth = viewSize.x * 0.5f;
			float halfHeight = viewSize.y * 0.5f;

			float padding = 100.0f;
			return (worldPos.x > viewCenter.x - halfWidth - padding &&
				worldPos.x < viewCenter.x + halfWidth + padding &&
				worldPos.y > viewCenter.y - halfHeight - padding &&
				worldPos.y < viewCenter.y + halfHeight + padding);
		}

		Vec2 GetScreenSize()
		{
			sf::RenderTexture* renderTexture = GEngine.GetRenderTexture();
			if (!renderTexture)
				return { 0.0f, 0.0f };

			sf::Vector2u size = renderTexture->getSize();
			return { static_cast<float>(size.x), static_cast<float>(size.y) };
		}

		Vec2 GetWorldBounds()
		{
			Scene* scene = GetCurrentScene();
			if (!scene)
				return GetScreenSize();

			Entity cameraEntity = scene->GetMainCameraEntity();
			if (!cameraEntity.IsValid() || !cameraEntity.Has<Camera2DComponent>())
				return GetScreenSize();

			auto& camera = cameraEntity.Get<Camera2DComponent>();
			const sf::View& view = camera.Camera.GetView();

			sf::Vector2f viewSize = view.getSize();
			return { viewSize.x, viewSize.y };
		}

		bool CheckCircleOverlap(const Vec3& posA, float radiusA, const Vec3& posB, float radiusB)
		{
			float distSq = DistanceSquared(posA, posB);
			float radiusSum = radiusA + radiusB;
			return distSq <= (radiusSum * radiusSum);
		}

		bool CheckAABBOverlap(const Vec3& posA, const Vec2& sizeA, const Vec3& posB, const Vec2& sizeB)
		{
			float halfWidthA = sizeA.x * 0.5f;
			float halfHeightA = sizeA.y * 0.5f;
			float halfWidthB = sizeB.x * 0.5f;
			float halfHeightB = sizeB.y * 0.5f;

			return (std::abs(posA.x - posB.x) < (halfWidthA + halfWidthB)) &&
				(std::abs(posA.y - posB.y) < (halfHeightA + halfHeightB));
		}

		bool IsPointInCircle(const Vec2& point, const Vec3& center, float radius)
		{
			float dx = point.x - center.x;
			float dy = point.y - center.y;
			return (dx * dx + dy * dy) <= (radius * radius);
		}

		bool IsPointInRect(const Vec2& point, const Vec3& rectPos, const Vec2& rectSize)
		{
			float halfWidth = rectSize.x * 0.5f;
			float halfHeight = rectSize.y * 0.5f;

			return (point.x >= rectPos.x - halfWidth &&
				point.x <= rectPos.x + halfWidth &&
				point.y >= rectPos.y - halfHeight &&
				point.y <= rectPos.y + halfHeight);
		}

		Vec2 GetCameraPosition()
		{
			Entity cameraEntity = GetMainCameraEntity();
			if (!cameraEntity.IsValid())
				return { 0.0f, 0.0f };

			Vec3 pos = GetPosition(cameraEntity);
			return { pos.x, pos.y };
		}

		void SetCameraPosition(const Vec2& position)
		{
			Entity cameraEntity = GetMainCameraEntity();
			if (!cameraEntity.IsValid())
				return;

			SetPosition(cameraEntity, Vec3(position.x, position.y, 0.0f));
		}

		float GetCameraZoom()
		{
			Entity cameraEntity = GetMainCameraEntity();
			if (!cameraEntity.IsValid() || !cameraEntity.Has<Camera2DComponent>())
				return 1.0f;

			return cameraEntity.Get<Camera2DComponent>().Camera.GetZoom();
		}

		void SetCameraZoom(float zoom)
		{
			Entity cameraEntity = GetMainCameraEntity();
			if (!cameraEntity.IsValid() || !cameraEntity.Has<Camera2DComponent>())
				return;

			cameraEntity.Get<Camera2DComponent>().Camera.SetZoom(zoom);
		}
	}
}


