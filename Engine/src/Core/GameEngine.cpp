#include "Core/GameEngine.h"

#include <iostream>

#include <imgui.h>
#include <imgui-SFML.h>

#include "Asset/Asset.h"
#include "Scene/Scene_Menu.h"

namespace Luden {

	GameEngine* GameEngine::s_Instance = nullptr;

	GameEngine::GameEngine(const std::string& assetPath) {
		Init(assetPath);
	}

	GameEngine::~GameEngine()
	{
		ImGui::SFML::Shutdown();
	}

	void GameEngine::Init(const std::string& assetPath) {
		m_Assets.LoadFromFile(assetPath);

		m_Window.create(sf::VideoMode(sf::Vector2u(1280, 720)), "Luden Engine");
		m_Window.setFramerateLimit(60);

		if (!ImGui::SFML::Init(m_Window)) {
			std::cerr << "ImGui-SFML initialization failed!" << std::endl;
		}

		// Default scene
		ChangeScene("MainMenu", std::make_shared<Scene_Menu>(this));
	}

	void GameEngine::Initialize(const std::string& assetPath) 
	{
		if (!s_Instance) 
		{
			s_Instance = new GameEngine(assetPath);
		}
	}

	void GameEngine::Run() {
		while (IsRunning()) {
			ProcessInput();
			ImGui::SFML::Update(m_Window, m_DeltaClock.restart());
			Update();
			ImGui::SFML::Render(m_Window);
			m_Window.display();
		}
	}

	void GameEngine::Update() {
		GetCurrentScene()->Update();
	}

	void GameEngine::ProcessInput()
	{
		while (const std::optional<sf::Event> event = m_Window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(m_Window, *event);

			if (event->is<sf::Event::Closed>())
			{
				Quit();
				return;
			}

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
					Quit();

				const auto& actionMap = GetCurrentScene()->GetActionMap();
				auto keyCode = static_cast<int>(keyPressed->code);
				auto it = actionMap.find(keyCode);
				if (it != actionMap.end())
					GetCurrentScene()->DoAction(Action(it->second, "START"));
			}

			if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
			{
				const auto& actionMap = GetCurrentScene()->GetActionMap();
				auto keyCode = static_cast<int>(keyReleased->code);
				auto it = actionMap.find(keyCode);
				if (it != actionMap.end())
					GetCurrentScene()->DoAction(Action(it->second, "END"));
			}

			if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
			{
				Math::Vec2 pos = { static_cast<float>(mouseMoved->position.x), static_cast<float>(mouseMoved->position.y) };
				GetCurrentScene()->DoAction(Action("MOUSE_MOVE", pos));
			}

			if (ImGui::GetIO().WantCaptureMouse)
				continue;

			if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				Math::Vec2 pos = { static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y) };
				std::string button;
				switch (mousePressed->button)
				{
				case sf::Mouse::Button::Left: button = "LEFT_CLICK"; break;
				case sf::Mouse::Button::Middle: button = "MIDDLE_CLICK"; break;
				case sf::Mouse::Button::Right: button = "RIGHT_CLICK"; break;
				default: break;
				}

				if (!button.empty())
				{
					GetCurrentScene()->DoAction(Action(button, "START", pos));
				}
			}

			if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
			{
				Math::Vec2 pos = { static_cast<float>(mouseReleased->position.x), static_cast<float>(mouseReleased->position.y) };
				std::string button;
				switch (mouseReleased->button)
				{
				case sf::Mouse::Button::Left: button = "LEFT_CLICK"; break;
				case sf::Mouse::Button::Middle: button = "MIDDLE_CLICK"; break;
				case sf::Mouse::Button::Right: button = "RIGHT_CLICK"; break;
				default: break;
				}
				if (!button.empty())
					GetCurrentScene()->DoAction(Action(button, "END", pos));
			}
		}
	}

	bool GameEngine::IsRunning() const 
	{
		return m_IsRunning && m_Window.isOpen();
	}

	void GameEngine::Quit() 
	{
		m_IsRunning = false;
		m_Window.close();
	}

	void GameEngine::Shutdown() 
	{
		if (s_Instance) {
			delete s_Instance;
			s_Instance = nullptr;
		}
	}

	void GameEngine::ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent) 
	{
		m_CurrentSceneName = name;
		m_SceneMap[name] = std::move(scene);
	}

	GameEngine& GameEngine::Get() 
	{
		if (!s_Instance) 
		{
			throw std::runtime_error("GameEngine not initialized! Use Initialize() first.");
		}
		return *s_Instance;
	}

	std::shared_ptr<Scene> GameEngine::GetCurrentScene() {
		return m_SceneMap.at(m_CurrentSceneName);
	}

	sf::RenderWindow& GameEngine::GetWindow() {
		return m_Window;
	}

	Assets& GameEngine::GetAssets() {
		return m_Assets;
	}
}
