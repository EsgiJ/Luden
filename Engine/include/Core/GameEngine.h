#pragma once

#include "ECS/ISystem.h"
#include "EngineAPI.h"
#include "Scene/Scene.h"
#include "Resource/ResourceManager.h"

#include <map>
#include <memory>
#include <string>

#include <imgui_internal.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

namespace Luden {

	using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

	class ENGINE_API GameEngine {
	public:
		GameEngine(const GameEngine&) = delete;
		GameEngine& operator=(const GameEngine&) = delete;

		static void Initialize(sf::RenderWindow& window, ImGuiContext* context, const std::string& resourcePath, bool headless);
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
		std::shared_ptr<ResourceManager> GetResourceManager() const;

	private:
		GameEngine(sf::RenderWindow& window, const std::string& resourcePath, bool headless);
		~GameEngine();

		template<typename T, typename... Args>
		T* AddSystem(Args&&... args)
		{
			static_assert(std::is_base_of<ISystem, T>::value, "T must inherit from ISystem");
			m_Systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
			return static_cast<T*>(m_Systems.back().get());
		}
	private:
		static GameEngine* s_Instance;

		void Init(const std::string& resourcePath, bool headless);

		sf::RenderWindow* m_Window = nullptr;
		sf::Clock m_DeltaClock;
		std::shared_ptr<ResourceManager> m_ResourceManager;

		std::string m_CurrentSceneName;
        SceneMap m_SceneMap;

		std::vector<std::unique_ptr<ISystem>> m_Systems;

        bool m_IsRunning = true;
        bool m_Headless = false;
    };
}
