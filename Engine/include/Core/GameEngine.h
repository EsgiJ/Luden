#pragma once

#include <map>
#include <memory>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <rttr/registration_friend.h>
#include <rttr/rttr_enable.h>

#include "Asset/Asset.h"
#include "EngineAPI.h"
#include "Scene/Scene.h"


namespace Luden {

	using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

	class ENGINE_API GameEngine {
	private:
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
		static GameEngine* s_Instance;

		GameEngine(const std::string& assetPath);
		~GameEngine();

		void Init(const std::string& assetPath);

		sf::RenderWindow m_Window;
		sf::Clock m_DeltaClock;
		Assets  m_Assets;

		std::string m_CurrentSceneName;
		SceneMap m_SceneMap;

		bool m_IsRunning = true;

		void ProcessInput();

	public:
		GameEngine(const GameEngine&) = delete;
		GameEngine& operator=(const GameEngine&) = delete;

		static void Initialize(const std::string& assetPath);
		static void Shutdown();
		void Run();
		bool IsRunning() const;
		void Quit();

		void ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent = false);
		
		std::shared_ptr<Scene> GetCurrentScene();
		static GameEngine& Get();
		sf::RenderWindow& GetWindow();
		Assets& GetAssets();

	private:
		void Update();
	};
} 
