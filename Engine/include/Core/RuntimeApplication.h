#pragma once
#include "Core/TimeStep.h"
#include "EngineAPI.h"
#include "Scene/Scene.h"
#include "Resource/ResourceManager.h"
#include "Project/Project.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Clock.hpp>
#include <memory>
#include <string>

namespace Luden {

	struct ENGINE_API ApplicationSpecification
	{
		std::string Name = "Luden Runtime";
		uint32_t WindowWidth = 1920;
		uint32_t WindowHeight = 1080;
		bool VSync = true;
		std::string WorkingDirectory;
		std::filesystem::path ProjectPath;
	};

	class NativeScriptModuleLoader;

	class ENGINE_API RuntimeApplication
	{
	public:
		RuntimeApplication(const ApplicationSpecification& spec);
		virtual ~RuntimeApplication();

		void Init();
		void Run();
		void Shutdown();

		std::shared_ptr<Scene> GetCurrentScene() const { return m_CurrentScene; }
		std::shared_ptr<EditorResourceManager> GetResourceManager() const { return m_ResourceManager; }
		sf::RenderWindow& GetWindow() { return *m_Window; }
		std::shared_ptr<sf::RenderTexture> GetRenderTexture() const { return m_RenderTexture; }

		bool IsRunning() const { return m_Running; }

		void LoadScene(ResourceHandle handle);
		void ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent = false);
	protected:
		void OpenProject();

		virtual void OnUpdate(TimeStep ts);

	protected:
		std::shared_ptr<EditorResourceManager> m_ResourceManager;
		std::shared_ptr<Scene> m_CurrentScene;
		std::string m_CurrentSceneName;
		ApplicationSpecification m_Specification;
		std::unique_ptr<sf::RenderWindow> m_Window;
		std::shared_ptr<sf::RenderTexture> m_RenderTexture;  
		std::unique_ptr<NativeScriptModuleLoader> m_NativeScriptModuleLoader;
		sf::Clock m_Clock;
		bool m_Running = true;
	};
}