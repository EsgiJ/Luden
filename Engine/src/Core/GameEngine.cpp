#include "Core/GameEngine.h"
#include <imgui-SFML.h>
#include <iostream>

namespace Luden {

	GameEngine* GameEngine::s_Instance = nullptr;

	GameEngine::GameEngine(const std::string& assetPath) {
		Init(assetPath);
	}

	GameEngine::~GameEngine()
	{
		SFML::Shutdown();
	}

	void GameEngine::Init(const std::string& assetPath) {
		m_Assets.LoadFromFile(assetPath);

		m_Window.create(sf::VideoMode(1280, 720), "Luden Engine");
		m_Window.setFramerateLimit(60);

		if (!ImGui::SFML::Init(m_Window)) {
			std::cerr << "ImGui-SFML initialization failed!" << std::endl;
		}

		// Default scene
		ChangeScene("MainMenu", std::make_shared<Scene>(this));
	}

	void GameEngine::Initialize(const std::string& assetPath) {
		if (!s_Instance) {  nbvcvbnmb
			s_Instance = new GameEngine(assetPath);
		}
	}

	GameEngine& GameEngine::Get() {
		if (!s_Instance) {
			throw std::runtime_error("GameEngine not initialized! Use Initialize() first.");
		}
		return *s_Instance;
	}

	void GameEngine::Shutdown() {
		if (s_Instance) {
			delete s_Instance;
			s_Instance = nullptr;
		}
	}

	void GameEngine::Run() {
		while (IsRunning()) {
			ProcessInput();
			ImGui::SFML::Update(m_Window, m_Clock.restart());

			if (RageEditor::Get().IsPlaying()) {
				Update();
			}

			RageEditor::Get().Update(); // ImGui Editor UI
			ImGui::SFML::Render(m_Window);
			m_Window.display();
		}
	}

	void GameEngine::ProcessInput() {
		/*while (const std::optional event = m_Window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			
			if (event.type == sf::Event::Closed) {
				Quit();
			}

			if (ImGui::GetIO().WantCaptureMouse) continue;
			GetCurrentScene()->HandleEvent(event);*/
		}
	}

	void GameEngine::Update() {
		GetCurrentScene()->Update();
	}

	void GameEngine::ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent) {
		m_CurrentSceneName = name;
		m_SceneMap[name] = std::move(scene);
	}

	std::shared_ptr<Scene> GameEngine::GetCurrentScene() {
		return m_SceneMap.at(m_CurrentSceneName);
	}

	bool GameEngine::IsRunning() const {
		return m_IsRunning && m_Window.isOpen();
	}

	void GameEngine::Quit() {
		m_IsRunning = false;
		m_Window.close();
	}

	sf::RenderWindow& GameEngine::GetWindow() {
		return m_Window;
	}

	std::vector<Asset>& GameEngine::GetAssets() {
		return m_Assets;
	}
}
