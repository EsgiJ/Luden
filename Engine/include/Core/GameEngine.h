#pragma once

#include <map>
#include <memory>
#include <string>

#include <imgui_internal.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include "Asset/Asset.h"
#include "EngineAPI.h"
#include "Scene/Scene.h"


namespace Luden {

	using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

	class ENGINE_API GameEngine {
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	public:
		GameEngine(const GameEngine&) = delete;
		GameEngine& operator=(const GameEngine&) = delete;

		static void Initialize(sf::RenderWindow& window, ImGuiContext* context, const std::string& assetPath, bool headless);
		static GameEngine& Get();
		static void Shutdown();

		void Run();
		bool IsRunning() const;
		void Quit();

		void Update(float dt);
		void Render(sf::RenderTarget& target);
		void ProcessInput();

		void ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent = false);
		std::shared_ptr<Scene> GetCurrentScene();
		sf::RenderWindow& GetWindow();
		Assets& GetAssets();

	private:
		GameEngine(sf::RenderWindow& window, const std::string& assetPath, bool headless);
		~GameEngine();

		static GameEngine* s_Instance;

		void Init(const std::string& assetPath, bool headless);

		sf::RenderWindow* m_Window = nullptr;
		sf::Clock m_DeltaClock;
		Assets m_Assets;

		std::string m_CurrentSceneName;
		SceneMap m_SceneMap;

		bool m_IsRunning = true;
	};
} 
