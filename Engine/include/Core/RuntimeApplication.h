#pragma once

#include "Application.h"
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

	class ENGINE_API RuntimeApplication : public Application
	{
	public:
		RuntimeApplication(const ApplicationSpecification& spec);
		virtual ~RuntimeApplication();

		void Init();
		void Run();
		void Shutdown();

		bool IsRunning() const { return m_Running; }

		virtual void ChangeScene(const std::string& sceneName) override;
		virtual void ReloadCurrentScene() override;
		virtual Scene* GetCurrentScene() override { return m_CurrentScene.get(); }

		std::shared_ptr<EditorResourceManager> GetResourceManager() const { return m_ResourceManager; }
		sf::RenderWindow& GetWindow() { return *m_Window; }
		std::shared_ptr<sf::RenderTexture> GetRenderTexture() const { return m_RenderTexture; }
	protected:
		void OpenProject();
		void LoadScene(ResourceHandle handle);
		void InternalChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent = true);

		virtual void OnUpdate(TimeStep ts);

	protected:
		std::shared_ptr<EditorResourceManager> m_ResourceManager = nullptr;
		std::shared_ptr<Scene> m_CurrentScene = nullptr;;
		std::string m_CurrentSceneName;
		ApplicationSpecification m_Specification;
		std::unique_ptr<sf::RenderWindow> m_Window = nullptr;
		std::shared_ptr<sf::RenderTexture> m_RenderTexture = nullptr;
		std::unique_ptr<NativeScriptModuleLoader> m_NativeScriptModuleLoader = nullptr;
		sf::Clock m_Clock;
		bool m_Running = true;
	};
}
