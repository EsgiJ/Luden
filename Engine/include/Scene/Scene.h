#pragma once

#include <map>
#include <memory>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/View.hpp>

#include "EngineAPI.h"
#include "ECS/EntityMemoryPool.h"
#include "ECS/EntityManager.h"
#include "Input/Action.h"
#include "Math/Vec2.h"
#include "Resource/Resource.h"

namespace Luden {

	class GameEngine;
	using ActionMap = std::map<int, std::string>;

	class ENGINE_API Scene : public Resource
	{
	public:
		Scene() = default;
		explicit Scene(GameEngine* gameEngine);
		virtual ~Scene() = default;

		virtual void Update() = 0;
		virtual void sRender(sf::RenderTarget& target) = 0;
		virtual void sDoAction(const Action& action) = 0;
		virtual void OnEnd() = 0;

		virtual void DoAction(const Action& action);
		void Simulate(size_t frames);
		void RegisterAction(sf::Keyboard::Key inputKey, const std::string& actionName);

		[[nodiscard]] float Width() const;
		[[nodiscard]] float Height() const;
		[[nodiscard]] size_t CurrentFrame() const;
		[[nodiscard]] bool HasEnded() const;
		[[nodiscard]] const ActionMap& GetActionMap() const;

		[[nodiscard]] EntityManager& GetEntityManager();
		[[nodiscard]] const EntityManager& GetEntityManager() const;
		void SetPaused(bool paused);
		void SetViewportSize(const sf::Vector2u& size);
		sf::Vector2u GetViewportSize() const { return m_ViewportSize; }
		sf::View& GetView() { return m_View; }
		const sf::View& GetView() const { return m_View; }

		void DrawLine(const Math::Vec2& p1, const Math::Vec2& p2);

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
	protected:
		std::shared_ptr<GameEngine> m_Game = nullptr;

		EntityManager m_EntityManager;
		ActionMap m_ActionMap;
		bool m_Paused = false;
		bool m_HasEnded = false;
		size_t m_CurrentFrame = 0;
		sf::Vector2u m_ViewportSize{ 0, 0 };
		sf::View m_View;
		std::string m_Name;
	};

}
