#pragma once

#include "Core/TimeStep.h"
#include "EngineAPI.h"
#include "Scene/Scene.h"
#include "Resource/ResourceManager.h"
#include "Project/Project.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <string>

namespace Luden {

	struct ENGINE_API ApplicationSpecification
	{
		std::string Name = "Luden";
		uint32_t WindowWidth = 1600, WindowHeight = 900;
		bool WindowDecorated = false;
		bool Fullscreen = false;
		bool VSync = true;
		std::string WorkingDirectory;
		bool StartMaximized = true;
		bool Resizable = true;
		bool EnableImGui = true;
		std::filesystem::path IconPath;
		bool Headless = false;
		std::filesystem::path m_ProjectPath;
	};

	class ENGINE_API RuntimeApplication 
	{
	public:
		RuntimeApplication(const ApplicationSpecification& spec);
		~RuntimeApplication();

		void OnInit();
		void OnShutdown();
		void OnUpdate(TimeStep ts);

		//Render
		void RenderTo(sf::RenderTarget& target);

		void OpenProject();

		// Scene Management
		void ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent = false);
		void LoadScene(ResourceHandle handle);
		std::shared_ptr<Scene> GetCurrentScene() const { return m_CurrentScene; }
		std::shared_ptr<RuntimeResourceManager> GetResourceManager() const { return m_ResourceManager; }

	private:
		std::shared_ptr<RuntimeResourceManager> m_ResourceManager;
		std::shared_ptr<ResourcePack> m_ResourcePack;

		std::shared_ptr<Scene> m_CurrentScene;
		std::string m_CurrentSceneName;

		ApplicationSpecification m_Specification;
		std::unique_ptr<sf::RenderWindow> m_Window; // only when (Headless == false)
	};

}
