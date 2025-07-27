#include "Core/GameEngine.h"

#include <iostream>

#include <imgui.h>
#include <imgui-SFML.h>

#include "Asset/AssetManager.h"
#include "Scene/Scene_Menu.h"
#include "Scene/Scene_Zelda.h"

namespace Luden {

	GameEngine* GameEngine::s_Instance = nullptr;

	GameEngine::GameEngine(sf::RenderWindow& window, const std::string& assetPath, bool headless)
		:       m_Window(&window), m_Headless(headless)
	{
		Init(assetPath, headless);
	}

	GameEngine::~GameEngine()
	{
		ImGui::SFML::Shutdown();
	}

       void GameEngine::Init(const std::string& assetPath, bool headless)
       {
               if (!m_Assets.LoadFromFile(assetPath))
               {
                       std::cerr << "Failed to load assets from " << assetPath << "\n";
                       m_IsRunning = false;
                       return;
               }

               // Default scene
               ChangeScene("Menu", std::make_shared<Scene_Menu>(this));

               if (headless)
                       return;

               if (!ImGui::SFML::Init(*m_Window))
               {
                       std::cerr << "ImGui-SFML initialization failed!" << std::endl;
               }
       }

	void GameEngine::Initialize(sf::RenderWindow& window, ImGuiContext* context, const std::string& assetPath, bool headless)
	{
		ImGui::SetCurrentContext(context);
		if (!s_Instance)
			s_Instance = new GameEngine(window, assetPath, headless);
	}

	void GameEngine::Run()
	{
		if (m_Headless)
			return;

		while (IsRunning() && m_Window && m_Window->isOpen())
		{
			ProcessInput();
			float dt = m_DeltaClock.restart().asSeconds();
			ImGui::SFML::Update(*m_Window, sf::seconds(dt));
			Update(dt);
			Render(*m_Window);
			ImGui::SFML::Render(*m_Window);
			m_Window->display();
		}
	}

	void GameEngine::Update(float)
	{
		GetCurrentScene()->Update();
	}

	void GameEngine::ProcessInput()
	{
		if (ImGui::GetCurrentContext() == nullptr)
		{
			throw std::runtime_error("Imgui Context doesn't exist!");
			return;
		}

		while (const std::optional<sf::Event> event = GetWindow().pollEvent()) 
		{

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
			if (!ImGui::GetIO().WantCaptureMouse)
				continue;

			if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
			{
				Math::Vec2 pos = { static_cast<float>(mouseMoved->position.x), static_cast<float>(mouseMoved->position.y) };
				GetCurrentScene()->DoAction(Action("MOUSE_MOVE", pos));
			}

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
		return m_IsRunning;
	}

	void GameEngine::Quit() 
	{
		m_IsRunning = false;
	}

	void GameEngine::Render(sf::RenderTarget& target)
	{
		if (auto scene = GetCurrentScene())
		{
			scene->SetViewportSize(target.getSize());
			target.setView(scene->GetView());
			scene->sRender(target);
		}
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
		if (!m_Window)
			throw std::runtime_error("RenderWindow is not set!");
		return *m_Window;
	}

	AssetManager& GameEngine::GetAssets() {
		return m_Assets;
	}
}
