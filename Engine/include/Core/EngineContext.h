#pragma once

#include "EngineAPI.h"

namespace Luden
{
	class Scene;

	class ENGINE_API EngineContext
	{
	public:
		static EngineContext& Instance()
		{
			static EngineContext s_Instance;
			return s_Instance;
		}

		void SetActiveScene(Scene* scene) { m_ActiveScene = scene; }
		Scene* GetActiveScene() const { return m_ActiveScene; }

		void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
		float GetDeltaTime() const { return m_DeltaTime; }

		void SetGameTime(float gameTime) { m_GameTime = gameTime; }
		float GetGameTime() const { return m_GameTime; }

		float GetTimeScale() { return m_TimeScale; }
		void SetTimeScale(float scale) { m_TimeScale = scale; }

		EngineContext(const EngineContext&) = delete;
		EngineContext& operator =(EngineContext&) = delete;

	private:
		EngineContext() = default;

		Scene* m_ActiveScene = nullptr;

		float m_DeltaTime = 0.0f;
		float m_GameTime = 0.0f;
		float m_TimeScale = 1.0f;
	};

	#define GEngine EngineContext::Instance()
}