// Engine/src/Core/GameEngine.cpp

#include "GameEngine.h"
#include <iostream>

namespace Luden
{
	GameEngine::GameEngine() : m_IsInitialized(false), m_IsRunning(false)
	{
	}

	GameEngine::~GameEngine()
	{
		if (m_IsInitialized)
		{
			Shutdown();
		}
	}

	GameEngine& GameEngine::Get()
	{
		static GameEngine instance;
		return instance;
	}

	void GameEngine::Initialize(unsigned int width, unsigned int height, const std::string& title)
	{
		if (m_IsInitialized)
			return;
		std::cout << "[Engine] Initializing GameEngine..." << std::endl;

	}

	void GameEngine::Run()
	{
		sf::Clock clock;
		while (m_IsRunning && m_Window.isOpen())
		{
			float deltaTime = clock.restart().asSeconds();
			ProcessEvents();
			Update(deltaTime);
			Render();
		}
	}

	void GameEngine::ProcessEvents()
	{

	}

	void GameEngine::Update(float deltaTime)
	{
		if (!m_IsInitialized)
			return;

		// Game logic update placeholder
		std::cout << "[Engine] Updating GameEngine: deltaTime = " << deltaTime << std::endl;
	}

	void GameEngine::Render()
	{
		if (!m_IsInitialized)
			return;

		m_Window.clear(sf::Color::Black);

		// TODO: Render game objects here...

		m_Window.display();
	}

	void GameEngine::Shutdown()
	{
		std::cout << "[Engine] Shutting down GameEngine..." << std::endl;
		if (m_Window.isOpen())
			m_Window.close();

		m_IsRunning = false;
		m_IsInitialized = false;
	}

	sf::RenderWindow& GameEngine::GetWindow()
	{
		return m_Window;
	}

	bool GameEngine::IsRunning() const
	{
		return m_IsRunning;
	}
}
