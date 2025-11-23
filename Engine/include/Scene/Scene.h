#pragma once

#include "EngineAPI.h"
#include "ECS/EntityMemoryPool.h"
#include "ECS/EntityManager.h"
#include "ECS/Entity.h"
#include <glm/vec2.hpp>
#include "Resource/Resource.h"
#include "Core/UUID.h"
#include "Core/TimeStep.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_set>

#include <SFML/Window.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <box2d/box2d.h>

namespace Luden {
;
	class ENGINE_API Scene : public Resource {
	public:
		Scene(const std::string& name = "Untitled");
		virtual ~Scene();

		// Update & Render
		virtual void OnUpdateRuntime(TimeStep ts, std::shared_ptr<sf::RenderTexture> renderTexture);
		virtual void OnUpdateEditor(TimeStep ts, std::shared_ptr<sf::RenderTexture> renderTexture);
		virtual void OnRenderRuntime(std::shared_ptr<sf::RenderTexture> target);
		virtual void OnRenderEditor(std::shared_ptr<sf::RenderTexture> target);

		// Runtime
		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		//Box2D
		void OnPhysics2DInit();
		void OnPhysics2DUpdate(TimeStep ts);
		void OnPhysics2DStop();

		// Input
		void OnEvent(const std::optional<sf::Event>& evt);

		// Entity management
		Entity CreateEntity(const std::string& tag = "");
		Entity CreateChildEntity(Entity parent, const std::string& name = "");
		Entity CreateEntity(const std::string& tag, UUID entityID);
		Entity CreateChildEntity(Entity parent, const std::string& name, UUID entityID);

		Entity DuplicateEntity(Entity& entity);

		void DestroyEntity(const Entity& entity);
		void DestroyEntity(const EntityID& entityID);

		void ParentEntity(Entity& entity, Entity& parent);
		void UnparentEntity(Entity& entity);

		Entity GetEntityWithUUID(const UUID& uuid) const;
		Entity TryGetEntityWithUUID(const UUID& uuid) const;
		Entity TryGetEntityWithTag(const std::string& tag) const;

		template<typename T>
		void CopyComponentIfExists(Entity dest, Entity source)
		{
			if (source.Has<T>())
			{
				auto& component = source.Get<T>();
				dest.Add<T>(component);
			}
		}
		EntityManager& GetEntityManager() { return m_EntityManager; }
		const EntityManager& GetEntityManager() const { return m_EntityManager; }

		// Viewport
		void SetViewportSize(uint32_t width, uint32_t height);
		uint32_t GetViewportWidth() const { return m_ViewportWidth; }
		uint32_t GetViewportHeight() const { return m_ViewportHeight; }
		sf::View& GetView() { return m_View; }
		const sf::View& GetView() const { return m_View; }

		float Width() const;
		float Height() const;

		// Utils
		void DrawLine(const glm::vec2& p1, const glm::vec2& p2);
		std::unordered_set<ResourceHandle> GetResourceList();

		// Metadata
		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		UUID GetUUID() const { return Handle; }

		void SetPaused(bool paused) { m_Paused = paused; }
		bool IsPaused() const { return m_Paused; }

		static ResourceType GetStaticType() { return ResourceType::Scene; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }

		bool IsPlaying() { return m_IsPlaying; }

	private:
		EntityManager m_EntityManager;

		std::string m_Name;

		bool m_IsPlaying = false;
		bool m_ShouldSimulate = false;
		bool m_Paused = false;

		size_t m_CurrentFrame = 0;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;
		sf::View m_View;

		//Physics2D
		b2WorldId m_PhysicsWorldId = b2_nullWorldId;
		const float m_PhysicsScale = 100.0f; // 1 meter = 100 pixel
		const b2Vec2 m_Gravity = { 0.0f, -10.0f };
		const int m_SubStepCount = 4; 
	};

}
