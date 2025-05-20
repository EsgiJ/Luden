#pragma once
#pragma once

#include <map>
#include <memory>
#include <string>

#include <SFML/Window.hpp>
#include <rttr/registration_friend.h>
#include <rttr/rttr_enable.h>

#include "EngineAPI.h"
#include "ECS/EntityMemoryPool.h"
#include "ECS/EntityManager.h"
#include "Input/Action.h"
#include "Math/Vec2.h"

namespace Luden {

	class GameEngine;
	using ActionMap = std::map<int, std::string>;

	class ENGINE_API Scene {
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	protected:
		GameEngine* m_Game = nullptr;

		EntityManager m_EntityManager;
		ActionMap m_ActionMap;
		bool m_Paused = false;
		bool m_HasEnded = false;
		size_t m_CurrentFrame = 0;
		sf::RenderTexture m_ViewportTexture;

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

		void DrawLine(const Math::Vec2& p1, const Math::Vec2& p2);
	};

}
