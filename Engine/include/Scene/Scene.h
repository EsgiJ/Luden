#pragma once
#pragma once

#include <map>
#include <memory>
#include <string>

#include "Core/Action.h"
#include "ECS/EntityMemoryPool.h"
#include "EngineAPI.h"
#include "Math/Vec2.h"

namespace Luden {

	class GameEngine;
	using ActionMap = std::map<int, std::string>;

	class ENGINE_API Scene {
	protected:
		GameEngine* m_Game = nullptr;
		EntityMemoryPool m_EntityManager;

		ActionMap m_ActionMap;
		bool m_Paused = false;
		bool m_HasEnded = false;
		size_t m_CurrentFrame = 0;

		virtual void OnEnd() = 0;
		void SetPaused(bool paused);

	public:
		Scene() = default;
		explicit Scene(GameEngine* gameEngine);
		virtual ~Scene() = default;

		virtual void Update() = 0;
		virtual void HandleAction(const Action& action) = 0;
		virtual void Render() = 0;
		virtual void sDoAction(const Action& action) = 0;

		virtual void DoAction(const Action& action);
		void Simulate(size_t frames);
		void RegisterAction(int inputKey, const std::string& actionName);

		[[nodiscard]] float Width() const;
		[[nodiscard]] float Height() const;
		[[nodiscard]] size_t CurrentFrame() const;
		[[nodiscard]] bool HasEnded() const;
		[[nodiscard]] const ActionMap& GetActionMap() const;

		void DrawLine(const Vec2& p1, const Vec2& p2);
	};

}
