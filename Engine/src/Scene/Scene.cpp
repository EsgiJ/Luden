#include "Scene/Scene.h"
#include "Core/RuntimeApplication.h"
#include "Project/Project.h"
#include "NativeScript/ScriptableEntity.h"
#include "Input/InputManager.h"
#include "Core/EngineContext.h"
#include "Debug/DebugManager.h"
#include "Graphics/AnimationManager.h"
#include "Graphics/Sprite.h"
#include "Physics2D/Physics2DManager.h"

#include <iostream>

#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include "Physics2D/CollisionChannelRegistry.h"

namespace Luden {

	Scene::Scene(const std::string& name)
		: m_Name(name) 
	{
	}

	Scene::~Scene() 
	{
		InputManager::Instance().ClearAllInput();
	}

	void Scene::OnUpdateRuntime(TimeStep ts, std::shared_ptr<sf::RenderTexture> renderTexture) {


		SetViewportSize(renderTexture->getSize().x, renderTexture->getSize().y);

		Entity cameraEntity = GetMainCameraEntity();

		if (!cameraEntity.IsValid())
			return;

		Camera2D& camera = cameraEntity.Get<Camera2DComponent>();
		camera.SetAttachedEntity(&cameraEntity);
		camera.SetViewportSize({ (float)m_ViewportWidth, (float)m_ViewportHeight });
		camera.Update();

		OnRenderRuntime(renderTexture, camera);

		GEngine.SetDeltaTime(static_cast<float>(ts));
		GEngine.SetGameTime(GEngine.GetGameTime() + static_cast<float>(ts) * GEngine.GetTimeScale());

		if (m_Paused)
			return;

		for (auto& entity : GetEntityManager().GetEntities())
		{
			if (entity.Has<NativeScriptComponent>())
			{
				auto& nsc = entity.Get<NativeScriptComponent>();

				if (nsc.Instance)
				{
					nsc.Instance->OnUpdate(ts);
				}
			}
		}

		m_PhysicsManager.Update(ts);
		InputManager::Instance().Update(ts, m_EntityManager);
		DebugManager::Instance().Update(ts);
		AnimationManager::Instance().Update(ts);
		m_EntityManager.Update(ts);
	}

	void Scene::OnUpdateEditor(TimeStep ts, std::shared_ptr<sf::RenderTexture> renderTexture, Camera2D& editorCamera)
	{
		SetViewportSize(renderTexture->getSize().x, renderTexture->getSize().y);

		editorCamera.SetViewportSize({ (float)m_ViewportWidth, (float)m_ViewportHeight });
		editorCamera.Update();

		OnRenderEditor(renderTexture, editorCamera);
		AnimationManager::Instance().Update(ts);
		m_PhysicsManager.Update(ts);

		m_EntityManager.Update(ts);
	}

	void Scene::OnRenderRuntime(std::shared_ptr<sf::RenderTexture> target, Camera2D& runtimeCamera)
	{
		target->clear(sf::Color(32, 32, 32));

		target->setView(runtimeCamera.GetView());

		for (auto& e : m_EntityManager.GetEntities())
		{
			if (!e.Has<TransformComponent>())
				continue;

			auto& transform = e.Get<TransformComponent>();

			if (e.Has<SpriteAnimatorComponent>())
			{
				RenderAnimatedEntity(e, transform, target);
			}
			else if (e.Has<SpriteRendererComponent>())
			{
				RenderStaticSprite(e, transform, target);
			}
		}

		DebugManager::Instance().Render(target);
		DebugManager::Instance().DebugDrawPhysics2D(m_PhysicsManager.GetPhysicsWorldId());
	}

	void Scene::OnRenderEditor(std::shared_ptr<sf::RenderTexture> target, Camera2D& editorCamera)
	{
		target->clear(sf::Color(32, 32, 32));

		target->setView(editorCamera.GetView());

		for (auto& e : m_EntityManager.GetEntities())
		{
			if (!e.Has<TransformComponent>())
				continue;

			auto& transform = e.Get<TransformComponent>();

			if (e.Has<SpriteAnimatorComponent>())
			{
				RenderAnimatedEntity(e, transform, target);
			}
			else if (e.Has<SpriteRendererComponent>())
			{
				RenderStaticSprite(e, transform, target);
			}
		}

		DebugManager::Instance().DebugDrawPhysics2D(m_PhysicsManager.GetPhysicsWorldId());
		DebugManager::Instance().Render(target);
	}

	void Scene::RenderStaticSprite(Entity& e, TransformComponent& transform, std::shared_ptr<sf::RenderTexture> target)
	{
		auto& spriteComp = e.Get<SpriteRendererComponent>();
		if (spriteComp.spriteHandle == 0)
			return;

		auto sprite = ResourceManager::GetResource<Sprite>(spriteComp.spriteHandle);
		if (!sprite) return;

		auto texture = ResourceManager::GetResource<Texture>(sprite->GetTextureHandle());
		if (!texture) return;

		sf::Sprite sfSprite(texture->GetTexture());

		if (!sprite->UsesFullTexture())
		{
			sfSprite.setTextureRect(sprite->GetTextureRect());
		}

		sf::FloatRect bounds = sfSprite.getLocalBounds();
		sfSprite.setOrigin(
			{ bounds.size.x * sprite->GetPivot().x,
			bounds.size.y * sprite->GetPivot().y }
		);

		sfSprite.setPosition({ transform.Translation.x, transform.Translation.y });
		sfSprite.setScale({ transform.Scale.x, transform.Scale.y });
		sfSprite.setRotation(sf::degrees(transform.angle));
		sfSprite.setColor(spriteComp.tint);

		target->draw(sfSprite);
	}

	void Scene::RenderAnimatedEntity(Entity& e, TransformComponent& transform, std::shared_ptr<sf::RenderTexture> target)
	{
		auto& animator = e.Get<SpriteAnimatorComponent>();

		// Validations
		if (animator.animationHandles.empty()) return;
		if (animator.currentAnimationIndex >= animator.animationHandles.size()) return;

		auto animation = ResourceManager::GetResource<Animation>(animator.animationHandles[animator.currentAnimationIndex]); 

		if (!animation || animation->GetFrameCount() == 0) return;

		if (animator.currentFrame >= animation->GetFrameCount())
			animator.currentFrame = 0;

		const auto& frame = animation->GetFrame(animator.currentFrame);

		auto sprite = ResourceManager::GetResource<Sprite>(frame.spriteHandle); 
		if (!sprite) return;

		auto texture = ResourceManager::GetResource<Texture>(sprite->GetTextureHandle());  
		if (!texture) return;

		sf::Sprite sfSprite(texture->GetTexture());

		if (!sprite->UsesFullTexture())
		{
			sfSprite.setTextureRect(sprite->GetTextureRect());
		}

		sf::FloatRect bounds = sfSprite.getLocalBounds();
		sfSprite.setOrigin(
			{ bounds.size.x * sprite->GetPivot().x + frame.offset.x,
			bounds.size.y * sprite->GetPivot().y + frame.offset.y}
		);

		sfSprite.setPosition({ transform.Translation.x, transform.Translation.y });
		sfSprite.setScale({ transform.Scale.x, transform.Scale.y });
		sfSprite.setRotation(sf::degrees(transform.angle));
		sfSprite.setColor(animator.tint);

		target->draw(sfSprite);
	}

	void Scene::OnRuntimeStart()
	{
		m_IsPlaying = true;

		m_EntityManager.Update(0.0f);

		GEngine.SetActiveScene(this);

		std::filesystem::path channelsPath = Project::GetActiveResourceDirectory() / "CollisionChannels.dat";
		if (std::filesystem::exists(channelsPath))
		{
			CollisionChannelRegistry::Instance().Deserialize(channelsPath);
		}

		for (auto& entity : GetEntityManager().GetEntities())
		{
			if (entity.Has<NativeScriptComponent>())
			{
				auto& nsc = entity.Get<NativeScriptComponent>();

				nsc.CreateInstance(entity);
			}
		}
		m_PhysicsManager.Init(this, m_ViewportWidth, m_ViewportHeight);
	}

	void Scene::OnRuntimeStop()
	{
		m_IsPlaying = false;

		if (GEngine.GetActiveScene() == this)
		{
			GEngine.SetActiveScene(nullptr);
		}

		for (auto& entity : GetEntityManager().GetEntities())
		{
			if (entity.Has<NativeScriptComponent>())
			{
				auto& nsc = entity.Get<NativeScriptComponent>();
				nsc.DestroyInstance();
			}

			if (entity.Has<InputComponent>())
			{

				entity.Get<InputComponent>().UnbindAll();
			}
		}


		m_PhysicsManager.Shutdown();
		InputManager::Instance().ClearAllInput();
	}

	void Scene::OnSimulationStart()
	{
		m_PhysicsManager.Init(this, m_ViewportWidth, m_ViewportHeight);
	}

	void Scene::OnSimulationStop()
	{
		m_PhysicsManager.Shutdown();
	}

	void Scene::OnEvent(const std::optional<sf::Event>& evt)
	{
		if (!m_IsPlaying && !m_ShouldSimulate)
			return;

		InputManager::Instance().ProcessEvent(*evt, m_EntityManager);
	}

	// Entity Management
	Entity Scene::CreateEntity(const std::string& tag) 
	{
		return CreateChildEntity({}, tag);
	}

	Entity Scene::CreateEntity(const std::string& tag, UUID entityID)
	{
		return CreateChildEntity({}, tag, entityID);
	}

	Entity Scene::CreateChildEntity(Entity parent, const std::string& name)
	{
		auto childEntity = m_EntityManager.AddEntity(name, this);
		childEntity.Add<TransformComponent>();

		childEntity.Add<RelationshipComponent>();

		if (parent.IsValid())
			childEntity.SetParent(parent);

		return childEntity;
	}

	Entity Scene::CreateChildEntity(Entity parent, const std::string& name, UUID entityID)
	{
		auto childEntity = m_EntityManager.AddEntity(name, entityID, this);
		childEntity.Add<TransformComponent>();

		childEntity.Add<RelationshipComponent>();

		if (parent.IsValid())
			childEntity.SetParent(parent);

		return childEntity;
	}

	Entity Scene::DuplicateEntity(Entity& entity)
	{
		auto parentNewEntity = [&entity, scene = this](Entity newEntity)
			{
				if (auto parent = entity.GetParent(); parent.IsValid())
				{
					newEntity.SetParentUUID(parent.UUID());
					parent.Children().push_back(newEntity.UUID());
				}
			};

		if (entity.Has<PrefabComponent>())
		{
			auto& prefabComp = entity.Get<PrefabComponent>();
			auto prefab = ResourceManager::GetResource<Prefab>(prefabComp.PrefabID);

			if (!prefab)
			{
				Entity newEntity = CreateEntity(entity.Tag() + "_copy");
				CopyAllComponents(newEntity, entity, false);
				parentNewEntity(newEntity);
				return newEntity;
			}

			const auto& entityTransform = entity.Get<TransformComponent>();
			glm::vec3 rotation(entityTransform.angle, 0.0f, 0.0f);
			Entity prefabInstance = Instantiate(prefab, &entityTransform.Translation, &rotation, &entityTransform.Scale);
			parentNewEntity(prefabInstance);
			return prefabInstance;
		}

		Entity newEntity = CreateEntity(entity.Tag() + "_copy");

		CopyAllComponents(newEntity, entity, false);

		auto childIds = entity.Children();
		for (auto childId : childIds)
		{
			Entity childEntity = GetEntityWithUUID(childId);
			Entity childDuplicate = DuplicateEntity(childEntity);

			UnparentEntity(childDuplicate);

			childDuplicate.SetParentUUID(newEntity.UUID());
			newEntity.Children().push_back(childDuplicate.UUID());
		}

		parentNewEntity(newEntity);
		return newEntity;
	}

	Entity Scene::CreatePrefabEntity(Entity entity, Entity parent, const glm::vec3* translation,
		const glm::vec3* rotation, const glm::vec3* scale)
	{
		if (entity.Has<PrefabComponent>())
		{
			auto& prefabComp = entity.Get<PrefabComponent>();
			auto nestedPrefab = ResourceManager::GetResource<Prefab>(prefabComp.PrefabID);

			if (nestedPrefab)
			{
				return Instantiate(nestedPrefab, translation, rotation, scale);
			}
			return {};
		}

		Entity newEntity = CreateEntity(entity.Tag());
		if (parent.IsValid())
			newEntity.SetParent(parent);

		if (entity.Has<TransformComponent>())
		{
			auto& srcTransform = entity.Get<TransformComponent>();
			auto& newTransform = newEntity.Add<TransformComponent>(srcTransform);

			if (translation)
				newTransform.Translation = *translation;
			if (rotation)
				newTransform.angle = rotation->x; 
			if (scale)
				newTransform.Scale = *scale;
		}

		CopyAllComponents(newEntity, entity, true);

		for (auto childId : entity.Children())
		{
			Entity childEntity = entity.GetScene()->GetEntityWithUUID(childId);
			CreatePrefabEntity(childEntity, newEntity, nullptr, nullptr, nullptr);
		}

		if (m_IsPlaying)
		{
			if (newEntity.Has<NativeScriptComponent>())
			{
				auto& nsc = newEntity.Get<NativeScriptComponent>();
				nsc.CreateInstance(newEntity);
			}
		}

		return newEntity;
	}

	Entity Scene::Instantiate(std::shared_ptr<Prefab> prefab, const glm::vec3* translation, const glm::vec3* rotation, const glm::vec3* scale)
	{
		if (!prefab || !prefab->GetScene())
			return {};

		Entity result;

		for (auto& entity : prefab->GetScene()->GetEntityManager().GetEntities())
		{
			if (!entity.Has<RelationshipComponent>())
				continue;

			if (!entity.GetParent().IsValid())
			{
				result = CreatePrefabEntity(entity, {}, translation, rotation, scale);

				if (result.IsValid() && !result.Has<PrefabComponent>())
				{
					auto& prefabComp = result.Add<PrefabComponent>();
					prefabComp.PrefabID = prefab->Handle;
					prefabComp.EntityID = result.UUID();
				}

				break;
			}
		}

		return result;
	}

	Entity Scene::InstantiateChild(std::shared_ptr<Prefab> prefab, Entity parent, const glm::vec3* translation, const glm::vec3* rotation, const glm::vec3* scale)
	{
		if (!prefab || !prefab->GetScene() || !parent.IsValid())
			return {};

		Entity result;

		for (auto& entity : prefab->GetScene()->GetEntityManager().GetEntities())
		{
			if (!entity.Has<RelationshipComponent>())
				continue;

			if (!entity.GetParent().IsValid())
			{
				result = CreatePrefabEntity(entity, parent, translation, rotation, scale);

				if (result.IsValid() && !result.Has<PrefabComponent>())
				{
					auto& prefabComp = result.Add<PrefabComponent>();
					prefabComp.PrefabID = prefab->Handle;
					prefabComp.EntityID = result.UUID();
				}

				break;
			}
		}

		return result;
	}

	void Scene::CopyAllComponents(Entity dest, Entity source, bool skipTransformAndRelationship)
	{
		if (!skipTransformAndRelationship)
		{
			CopyComponentIfExists<TransformComponent>(dest, source);
			CopyComponentIfExists<RelationshipComponent>(dest, source);
		}

		CopyComponentIfExists<PrefabComponent>(dest, source);
		CopyComponentIfExists<DamageComponent>(dest, source);
		CopyComponentIfExists<DraggableComponent>(dest, source);
		CopyComponentIfExists<FollowPLayerComponent>(dest, source);
		CopyComponentIfExists<GravityComponent>(dest, source);
		CopyComponentIfExists<HealthComponent>(dest, source);
		CopyComponentIfExists<InputComponent>(dest, source);
		CopyComponentIfExists<Camera2DComponent>(dest, source);
		CopyComponentIfExists<RigidBody2DComponent>(dest, source);
		CopyComponentIfExists<BoxCollider2DComponent>(dest, source);
		CopyComponentIfExists<CircleCollider2DComponent>(dest, source);
		CopyComponentIfExists<NativeScriptComponent>(dest, source);
		CopyComponentIfExists<SpriteAnimatorComponent>(dest, source);
		CopyComponentIfExists<TextComponent>(dest, source);
		CopyComponentIfExists<SpriteRendererComponent>(dest, source);
		CopyComponentIfExists<InvincibilityComponent>(dest, source);
		CopyComponentIfExists<LifespanComponent>(dest, source);
		CopyComponentIfExists<PatrolComponent>(dest, source);
		CopyComponentIfExists<StateComponent>(dest, source);
	}

	void Scene::DestroyEntity(const Entity& entity) 
	{
		m_EntityManager.DestroyEntity(entity.UUID());
	}

	void Scene::DestroyEntity(const EntityID& entityID) 
	{
		m_EntityManager.DestroyEntity(entityID);
	}

	void Scene::ParentEntity(Entity& entity, Entity& parent)
	{
		if (entity.IsDescendantOf(entity))
		{
			UnparentEntity(parent);

			Entity newParent = TryGetEntityWithUUID(entity.GetParentUUID());
			if (newParent.IsValid())
			{
				UnparentEntity(entity);
				ParentEntity(parent, entity);
			}
		}
		else
		{
			Entity previousParent = TryGetEntityWithUUID(entity.GetParentUUID());

			if (previousParent)
				UnparentEntity(entity);
		}
		entity.SetParentUUID(parent.UUID());
		parent.Children().push_back(entity.UUID());

	}

	void Scene::UnparentEntity(Entity& entity)
	{
		Entity parent = TryGetEntityWithUUID(entity.GetParentUUID());
		if (!parent.IsValid())
			return;

		auto& parentChildren = parent.Children();
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.UUID()), parentChildren.end());

		entity.SetParentUUID(0);
	}

	Entity Scene::GetEntityWithUUID(const UUID& uuid) const 
	{
		if (!m_EntityManager.Exists(uuid))
		{
			throw std::runtime_error("Entity with UUID not found!");
		}
		return m_EntityManager.GetEntity(uuid);
	}

	Entity Scene::TryGetEntityWithUUID(const UUID& uuid) const
	{
		return m_EntityManager.TryGetEntityWithUUID(uuid);
	}

	Entity Scene::TryGetEntityWithTag(const std::string& tag) const
	{
		return m_EntityManager.TryGetEntityWithTag(tag);
	}

	std::vector<Entity>& Scene::FindAllEntitiesWithTag(const std::string& tag)
	{
		return m_EntityManager.GetEntityMap().at(tag);
	}

	Entity Scene::FindEntityByBodyId(b2BodyId bodyId)
	{
		for (const auto& entity : m_EntityManager.GetEntities())
		{
			if (entity.Has<RigidBody2DComponent>())
			{
				auto& rb = entity.Get<RigidBody2DComponent>();
				if (B2_ID_EQUALS(rb.RuntimeBodyId, bodyId))
				{
					return entity;
				}
			}
		}

		return {};
	}

	Entity Scene::FindEntityByShapeId(b2ShapeId shapeId)
	{
		for (const auto& entity : m_EntityManager.GetEntities())
		{
			if (entity.Has<CircleCollider2DComponent>())
			{
				auto& collider = entity.Get<CircleCollider2DComponent>();
				if (B2_ID_EQUALS(collider.RuntimeShapeId, shapeId))
				{
					return entity;
				}
			}
			else if (entity.Has<BoxCollider2DComponent>())
			{
				auto& collider = entity.Get<BoxCollider2DComponent>();
				if (B2_ID_EQUALS(collider.RuntimeShapeId, shapeId))
				{
					return entity;
				}
			}
		}

		return {};
	}

	// Viewport
	void Scene::SetViewportSize(uint32_t width, uint32_t height) 
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	Entity Scene::GetMainCameraEntity()
	{
		for (auto& entity : m_EntityManager.GetEntities())
		{
			if (entity.Has<TransformComponent>() && entity.Has<Camera2DComponent>())
			{
				if (entity.Get<Camera2DComponent>().Primary)
					return entity;
			}
		}
		return {};
	}

	float Scene::Width() const
	{
		return (float)m_ViewportWidth;
	}

	float Scene::Height() const
	{
		return (float)m_ViewportHeight;
	}

	// Utils
	void Scene::DrawLine(const glm::vec2& p1, const glm::vec2& p2) 
	{
		sf::Vertex line[] = {
			sf::Vertex(sf::Vector2f(p1.x, p1.y)),
			sf::Vertex(sf::Vector2f(p2.x, p2.y))
		};
	}

	std::unordered_set<ResourceHandle> Scene::GetResourceList()
	{
		std::unordered_set<ResourceHandle> resources;

		for (auto entity : GetEntityManager().GetEntities())
		{
			if (entity.Has<SpriteRendererComponent>())
				resources.insert(entity.Get<SpriteRendererComponent>().spriteHandle);

			if (entity.Has<TextComponent>())
				resources.insert(entity.Get<TextComponent>().fontHandle);

			if (entity.Has<SpriteAnimatorComponent>())
			{
				for (auto handle : entity.Get<SpriteAnimatorComponent>().animationHandles)
					resources.insert(handle);
			}

			if (entity.Has<NativeScriptComponent>())
				resources.insert(entity.Get<NativeScriptComponent>().ScriptHandle);
		}

		return resources;
	}

	b2WorldId Scene::GetPhysicsWorldId()
	{
		return m_PhysicsManager.GetPhysicsWorldId();
	}

	std::shared_ptr<Scene> Scene::CreateEmpty()
	{
		return std::make_shared<Scene>("Empty");
	}

}
