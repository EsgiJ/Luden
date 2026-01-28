#pragma once

#include "EngineAPI.h"

#include <glm/glm.hpp>

namespace sf
{
	class RenderWindow;
	class RenderTexture;
}

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

		void SetWindow(sf::RenderWindow* window) { m_Window = window; }
		sf::RenderWindow* GetWindow() const { return m_Window; }

		void SetRenderTexture(sf::RenderTexture* renderTexture) { m_RenderTexture = renderTexture; }
		sf::RenderTexture* GetRenderTexture() const { return m_RenderTexture; }

		void SetViewportBounds(const glm::vec2& position, const glm::vec2& size)
		{
			m_ViewportPosition = position;
			m_ViewportSize = size;
		}

		glm::vec2 GetViewportPosition() const { return m_ViewportPosition; }
		glm::vec2 GetViewportSize() const { return m_ViewportSize; }

		EngineContext(const EngineContext&) = delete;
		EngineContext& operator =(EngineContext&) = delete;

	private:
		EngineContext() = default;

		Scene* m_ActiveScene = nullptr;

		float m_DeltaTime = 0.0f;
		float m_GameTime = 0.0f;
		float m_TimeScale = 1.0f;

		glm::vec2 m_ViewportPosition{ 0.0f, 0.0f };  
		glm::vec2 m_ViewportSize{ 1920.0f, 1080.0f }; 

		sf::RenderWindow* m_Window = nullptr;
		sf::RenderTexture* m_RenderTexture = nullptr;
	};

	#define GEngine EngineContext::Instance()
}
