// Engine/include/Luden/Core/GameEngine.h
#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "EngineAPI.h"


namespace Luden 
{
	class ENGINE_API GameEngine
	{
	public:
		// Singleton access
		static GameEngine& Get();

		// Initialization and run
		void Initialize(unsigned int width, unsigned int height, const std::string& title);
		void Run();
		void Shutdown();

		// Window access
		sf::RenderWindow& GetWindow();
		bool IsRunning() const;

	private:
		GameEngine();
		~GameEngine();

		GameEngine(const GameEngine&) = delete;
		GameEngine& operator=(const GameEngine&) = delete;

		void ProcessEvents();
		void Update(float deltaTime);
		void Render();

	private:
		sf::RenderWindow m_Window;
		bool m_IsRunning = false;
		bool m_IsInitialized = false;
	};
}