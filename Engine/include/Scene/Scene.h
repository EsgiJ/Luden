#pragma once

#include "EngineAPI.h"
#include "ECS/EntityMemoryPool.h"
#include "ECS/EntityManager.h"
#include "ECS/Entity.h"
#include "Input/Action.h"
#include "Math/Vec2.h"
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

namespace Luden {

	using ActionMap = std::map<int, std::string>;

	class ENGINE_API Scene : public Resource {
	public:
		Scene(const std::string& name = "Untitled");
		virtual ~Scene();

		// Update & Render
		virtual void OnUpdateRuntime(TimeStep ts, std::shared_ptr<sf::RenderTexture> renderTexture);
		virtual void OnUpdateEditor(TimeStep ts, std::shared_ptr<sf::RenderTexture> renderTexture);
		virtual void OnRenderRuntime(std::shared_ptr<sf::RenderTexture> target);
		virtual void OnRenderEditor(std::shared_ptr<sf::RenderTexture> target);

		// Input
		void DoAction(const Action& action);
		void RegisterAction(sf::Keyboard::Key inputKey, const std::string& actionName);
		const ActionMap& GetActionMap() const { return m_ActionMap; }

		// Entity management
		Entity CreateEntity(const std::string& tag = "");
		Entity CreateChildEntity(Entity parent, const std::string& name = "");
		Entity DuplicateEntity(const Entity& entity);
		void DestroyEntity(const Entity& entity);
		void DestroyEntity(const EntityID& entityID);

		Entity GetEntityWithUUID(const UUID& uuid) const;
		Entity TryGetEntityWithUUID(const UUID& uuid);
		Entity TryGetEntityWithTag(const std::string& tag);

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
		void DrawLine(const Math::Vec2& p1, const Math::Vec2& p2);
		std::unordered_set<ResourceHandle> GetResourceList();

		// Metadata
		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		UUID GetUUID() const { return Handle; }

		void SetPaused(bool paused) { m_Paused = paused; }
		bool IsPaused() const { return m_Paused; }

		static ResourceType GetStaticType() { return ResourceType::Scene; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }

	private:
		EntityManager m_EntityManager;
		ActionMap m_ActionMap;

		std::string m_Name;

		bool m_IsPlaying = false;
		bool m_ShouldSimulate = false;
		bool m_Paused = false;

		size_t m_CurrentFrame = 0;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;
		sf::View m_View;
	};

}
