#pragma once

#include <string>
#include <memory>
#include <map>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include "Scene/Scene.h"
#include "Core/Assets.h"
#include "EngineAPI.h"

namespace Luden {

	using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

	class ENGINE_API GameEngine {
	private:
		static GameEngine* s_Instance;

		GameEngine(const std::string& assetPath);
		~GameEngine();

		void Init(const std::string& assetPath);

		sf::RenderWindow m_Window;
		sf::Clock m_Clock;
		std::vector<Asset> m_Assets;

		std::string m_CurrentSceneName;
		SceneMap m_SceneMap;

		bool m_IsRunning = true;

		void ProcessInput();

	public:
		static void Initialize(const std::string& assetPath);
		static GameEngine& Get();
		static void Shutdown();
		GameEngine(const GameEngine&) = delete;
		GameEngine& operator=(const GameEngine&) = delete;

		void Run();

		void ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent = false);
		std::shared_ptr<Scene> GetCurrentScene();
		bool IsRunning() const;
		void Quit();

		sf::RenderWindow& GetWindow();
		Assets& GetAssets();

	private:
		void Update();
	};

} // namespace Luden
