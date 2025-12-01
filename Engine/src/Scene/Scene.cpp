#include "Scene/Scene.h"
#include "Core/RuntimeApplication.h"
#include "Project/Project.h"
#include "NativeScript/ScriptableEntity.h"
#include "Input/InputManager.h"
#include "Core/EngineContext.h"

#include <iostream>

#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include "Debug/DebugManager.h"


namespace Luden {

	Scene::Scene(const std::string& name)
		: m_Name(name) 
	{
	}

	Scene::~Scene() 
	{
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
		OnPhysics2DUpdate(ts);
		InputManager::Instance().Update(ts, m_EntityManager);
		DebugManager::Instance().Update(ts);
		m_EntityManager.Update(ts);
	}

	void Scene::OnUpdateEditor(TimeStep ts, std::shared_ptr<sf::RenderTexture> renderTexture, Camera2D& editorCamera)
	{
		SetViewportSize(renderTexture->getSize().x, renderTexture->getSize().y);

		editorCamera.SetViewportSize({ (float)m_ViewportWidth, (float)m_ViewportHeight });
		editorCamera.Update();

		OnRenderEditor(renderTexture, editorCamera);

		OnPhysics2DUpdate(ts);

		m_EntityManager.Update(ts);

		if (m_Paused)
			return;

		if (m_ShouldSimulate) {
			//m_EntityManager.Update(ts);
		}
		else if (m_IsPlaying) {
			//m_EntityManager.Update(ts);
			// UpdateAnimation
		}
	}

	// Render
	void Scene::OnRenderRuntime(std::shared_ptr<sf::RenderTexture> target, Camera2D& runtimeCamera)
	{
		target->clear(sf::Color(255, 192, 122));

		target->setView(runtimeCamera.GetView());

		for (auto& e : m_EntityManager.GetEntities())
		{
			if (!e.Has<TransformComponent>())
				continue;

			auto& transform = e.Get<TransformComponent>();

			sf::Color c = sf::Color::White;

			if (e.Has<Animation2DComponent>())
			{
				auto& animationComp = e.Get<Animation2DComponent>();
				auto animation = std::static_pointer_cast<Graphics::Animation>(Project::GetResourceManager()->GetResource(animationComp.animationHandle));
				sf::Sprite sprite = animation->GetSprite();

				sprite.setPosition(sf::Vector2f(transform.Translation.x, transform.Translation.y));
				sprite.setScale(sf::Vector2f(transform.Scale.x, transform.Scale.y));
				sprite.setColor(c);

				target->draw(sprite);
			}
			else if (e.Has<TextureComponent>())
			{
				auto& textureComp = e.Get<TextureComponent>();
				auto texHandle = textureComp.textureHandle;

				auto textureRes = std::static_pointer_cast<Texture>(Project::GetResourceManager()->GetResource(texHandle));
				if (textureRes)
				{
					sf::Sprite sprite(textureRes->GetTexture());
					sprite.setPosition(sf::Vector2f(transform.Translation.x, transform.Translation.y));
					sprite.setScale(sf::Vector2f(transform.Scale.x, transform.Scale.y));
					sprite.setColor(c);

					target->draw(sprite);
				}
			}
		}

		DebugManager::Instance().Render(target);
	}

	void Scene::OnRenderEditor(std::shared_ptr<sf::RenderTexture> target, Camera2D& editorCamera)
	{
		target->clear(sf::Color(255, 192, 122));

		target->setView(editorCamera.GetView());

		for (auto& e : m_EntityManager.GetEntities())
		{
			if (!e.Has<TransformComponent>())
				continue;

			auto& transform = e.Get<TransformComponent>();

			sf::Color c = sf::Color::White;

			if (e.Has<Animation2DComponent>())
			{
				auto& animationComp = e.Get<Animation2DComponent>();
				auto animation = std::static_pointer_cast<Graphics::Animation>(Project::GetResourceManager()->GetResource(animationComp.animationHandle));
				sf::Sprite sprite = animation->GetSprite();

				sprite.setPosition(sf::Vector2f(transform.Translation.x, transform.Translation.y));
				sprite.setScale(sf::Vector2f(transform.Scale.x, transform.Scale.y));
				sprite.setColor(c);
				target->draw(sprite);
			}
			else if (e.Has<TextureComponent>())
			{
				auto& textureComp = e.Get<TextureComponent>();
				auto texHandle = textureComp.textureHandle;

				auto textureRes = std::static_pointer_cast<Texture>(Project::GetResourceManager()->GetResource(texHandle));
				if (textureRes)
				{
					sf::Sprite sprite(textureRes->GetTexture());
					sprite.setPosition(sf::Vector2f(transform.Translation.x, transform.Translation.y));
					sprite.setScale(sf::Vector2f(transform.Scale.x, transform.Scale.y));
					sprite.setColor(c);
					sprite.setRotation(sf::degrees(transform.angle));
					target->draw(sprite);
				}
			}


		}
		DebugManager::Instance().Render(target);

		//TODO: Editor overlay (gizmo, bounding box, grid)
	}

	void Scene::OnRuntimeStart()
	{
		m_IsPlaying = true;

		m_EntityManager.Update(0.0f);

		GEngine.SetActiveScene(this);

		for (auto& entity : GetEntityManager().GetEntities())
		{
			if (entity.Has<NativeScriptComponent>())
			{
				auto& nsc = entity.Get<NativeScriptComponent>();

				nsc.CreateInstance(entity);
			}
		}
		OnPhysics2DInit();
	}

	void Scene::OnRuntimeStop()
	{
		m_IsPlaying = false;

		if (GEngine.GetActiveScene() == this)
		{
			GEngine.SetActiveScene(nullptr);
		}

		OnPhysics2DStop();
		InputManager::Instance().ClearAllInput();

		for (auto& entity : GetEntityManager().GetEntities())
		{
			if (entity.Has<NativeScriptComponent>())
			{
				auto& nsc = entity.Get<NativeScriptComponent>();

				nsc.DestroyInstance();
			}
		}
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DInit();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnPhysics2DInit()
	{
		//TODO: Fix this hacky solution to update the entitiesToAdd
		m_EntityManager.Update(TimeStep(0.0f));

		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = m_Gravity;
		m_PhysicsWorldId = b2CreateWorld(&worldDef);
		int bodyCount = 0;

		for (auto& entity : GetEntityManager().GetEntities())
		{
			if (entity.Has<RigidBody2DComponent>() && entity.Has<TransformComponent>())
			{
				auto& rb2d = entity.Get<RigidBody2DComponent>();
				auto& transformComponent = entity.Get<TransformComponent>();

				b2BodyDef bodyDef = b2DefaultBodyDef();

				if (rb2d.BodyType == RigidBody2DComponent::Type::Static)
					bodyDef.type = b2_staticBody;
				else if (rb2d.BodyType == RigidBody2DComponent::Type::Kinematic)
					bodyDef.type = b2_kinematicBody;
				else if (rb2d.BodyType == RigidBody2DComponent::Type::Dynamic)
					bodyDef.type = b2_dynamicBody;
				
				bodyDef.position = b2Vec2(
					transformComponent.Translation.x / m_PhysicsScale,
					(m_ViewportHeight - transformComponent.Translation.y) / m_PhysicsScale
				);
				bodyDef.rotation = b2MakeRot(glm::radians(transformComponent.angle));

				if (rb2d.FixedRotation)
				{
					bodyDef.motionLocks.angularZ = true;
				}

				bodyDef.linearDamping = rb2d.LinearDrag; 
				bodyDef.angularDamping = rb2d.AngularDrag;
				bodyDef.gravityScale = rb2d.GravityScale;

				bodyDef.userData = (void*)(uintptr_t)entity.UUID(); 
				rb2d.RuntimeBodyId = b2CreateBody(m_PhysicsWorldId, &bodyDef);

				if (entity.Has<BoxCollider2DComponent>())
				{
					auto& bc2d = entity.Get<BoxCollider2DComponent>();
					b2BodyId bodyId = rb2d.RuntimeBodyId;

					float halfWidth = (bc2d.Size.x * transformComponent.Scale.x) / (2.0f * m_PhysicsScale);
					float halfHeight = (bc2d.Size.y * transformComponent.Scale.y) / (2.0f * m_PhysicsScale);

					b2Vec2 centerOffset = {
						bc2d.Offset.x / m_PhysicsScale,
						bc2d.Offset.y / m_PhysicsScale
					};

					b2Rot localRotation = b2MakeRot(0.0f);

					b2Polygon boxShape = b2MakeOffsetBox(
						(bc2d.Size.x * transformComponent.Scale.x) / (2.0f * m_PhysicsScale),
						(bc2d.Size.y * transformComponent.Scale.y) / (2.0f * m_PhysicsScale),
						centerOffset,
						localRotation
					);

					b2ShapeDef shapeDef = b2DefaultShapeDef();
					shapeDef.density = bc2d.Density;
					shapeDef.material.friction = bc2d.Friction;

					bc2d.RuntimeShapeId = b2CreatePolygonShape(bodyId, &shapeDef, &boxShape);
				}

				if (entity.Has<CircleCollider2DComponent>())
				{
					auto& cc2d = entity.Get<CircleCollider2DComponent>();
					b2BodyId bodyId = rb2d.RuntimeBodyId;

					float scale = glm::max(transformComponent.Scale.x, transformComponent.Scale.y);

					b2Circle circle =
					{
						{ cc2d.Offset.x / m_PhysicsScale, cc2d.Offset.y / m_PhysicsScale },
						(cc2d.Radius * scale) / m_PhysicsScale
					};

					b2ShapeDef shapeDef = b2DefaultShapeDef();

					shapeDef.density = cc2d.Density;
					shapeDef.material.friction = cc2d.Friction;

					cc2d.RuntimeShapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
				}
			}
		}
	}

	void Scene::OnPhysics2DUpdate(TimeStep ts)
	{
		if (!b2World_IsValid(m_PhysicsWorldId))
			return;

		b2World_Step(m_PhysicsWorldId, static_cast<float>(ts), m_SubStepCount);

		for (auto& entity : m_EntityManager.GetEntities())
		{
			if (entity.Has<RigidBody2DComponent>() && entity.Has<TransformComponent>())
			{
				auto& rb2d = entity.Get<RigidBody2DComponent>();
				auto& transform = entity.Get<TransformComponent>();
				b2BodyId bodyId = rb2d.RuntimeBodyId; 

				if (b2Body_IsValid(bodyId))
				{
					b2Vec2 position = b2Body_GetPosition(bodyId); 
					b2Rot rotation = b2Body_GetRotation(bodyId);

					float angle = atan2f(rotation.s, rotation.c);

					transform.Translation.x = position.x * m_PhysicsScale;
					transform.Translation.y = m_ViewportHeight - (position.y * m_PhysicsScale);
					transform.angle = glm::degrees(angle);
				}
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		if (b2World_IsValid(m_PhysicsWorldId))
		{
			b2DestroyWorld(m_PhysicsWorldId);
			m_PhysicsWorldId = b2_nullWorldId;
		}

		for (auto& e : m_EntityManager.GetEntities())
		{
			if (e.Has<RigidBody2DComponent>())
			{
				e.Get<RigidBody2DComponent>().RuntimeBodyId = b2_nullBodyId;
			}
		}
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

		Entity newEntity = CreateEntity((entity.Tag() + "_copy"));
		
		CopyComponentIfExists<RelationshipComponent>(newEntity, entity);
		CopyComponentIfExists<DamageComponent>(newEntity, entity);
		CopyComponentIfExists<DraggableComponent>(newEntity, entity);
		CopyComponentIfExists<FollowPLayerComponent>(newEntity, entity);
		CopyComponentIfExists<GravityComponent>(newEntity, entity);
		CopyComponentIfExists<HealthComponent>(newEntity, entity);
		CopyComponentIfExists<InputComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Animation2DComponent>(newEntity, entity);
		CopyComponentIfExists<TextComponent>(newEntity, entity);
		CopyComponentIfExists<TextureComponent>(newEntity, entity);
		CopyComponentIfExists<InvincibilityComponent>(newEntity, entity);
		CopyComponentIfExists<LifespanComponent>(newEntity, entity);
		CopyComponentIfExists<PatrolComponent>(newEntity, entity);
		CopyComponentIfExists<StateComponent>(newEntity, entity);
		CopyComponentIfExists<TransformComponent>(newEntity, entity);

		parentNewEntity(entity);

		return newEntity;
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
		std::unordered_set<ResourceHandle> resourceList;

		//Animation
		for (const auto& animation : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Animation))
		{
			resourceList.insert(animation);
		}

		//Font 
		for (const auto& font : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Font))
		{
			resourceList.insert(font);
		}

		//Audio
		for (const auto& audio : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Audio))
		{
			resourceList.insert(audio);
		}

		//Texture
		for (const auto& texture : Project::GetResourceManager()->GetAllResourcesWithType(ResourceType::Texture))
		{
			resourceList.insert(texture);
		}
		return resourceList;
	}

}
