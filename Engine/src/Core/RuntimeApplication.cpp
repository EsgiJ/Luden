#include "Core/RuntimeApplication.h"
#include "Project/Project.h"
#include "Scene/Scene.h"
#include "Resource/ResourceManager.h"

#include <iostream>

namespace Luden {

	RuntimeApplication::RuntimeApplication(const ApplicationSpecification& spec)
		: m_Specification(spec)
	{
		if (!m_Specification.Headless) 
		{
			sf::VideoMode vm({ m_Specification.WindowWidth, m_Specification.WindowHeight });
			m_Window = std::make_unique<sf::RenderWindow>(vm, m_Specification.Name);
		}
	}

	RuntimeApplication::~RuntimeApplication() 
	{
		OnShutdown();
	}

	void RuntimeApplication::OnInit() 
	{
		std::cout << "[RuntimeApplication] Initializing...\n";

		auto project = Project::Load("MyGame/MyGame.project");
		if (!project) 
		{
			std::cerr << "[RuntimeApplication] Failed to load project file!\n";
			return;
		}

		Project::SetActiveRuntime(project);

		m_ResourceManager = Project::GetRuntimeResourceManager();
		if (!m_ResourceManager) 
		{
			std::cerr << "[RuntimeApplication] ResourceManager not initialized!\n";
			return;
		}

		LoadStartScene();
	}

	void RuntimeApplication::OnShutdown() 
	{
		if (m_CurrentScene) 
		{
			m_CurrentScene->OnRuntimeStop();
			m_CurrentScene.reset();
		}
		m_ResourceManager.reset();

		if (m_Window && m_Window->isOpen())
			m_Window->close();
	}

	void RuntimeApplication::OnUpdate(TimeStep ts) 
	{
		if (!m_CurrentScene) return;

		m_CurrentScene->OnUpdateRuntime(ts);

		if (!m_Specification.Headless && m_Window) 
		{
			m_Window->clear();
			m_CurrentScene->OnRenderRuntime(*m_Window);
			m_Window->display();
		}
	}

	void RuntimeApplication::RenderTo(sf::RenderTarget& target) 
	{
		if (m_CurrentScene) {
			m_CurrentScene->OnRenderRuntime(target);
		}
	}

	void RuntimeApplication::ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent) 
	{
		if (endCurrent && m_CurrentScene)
		{
			m_CurrentScene->OnRuntimeStop();
		}

		m_CurrentScene = scene;
		m_CurrentSceneName = name;

		if (m_CurrentScene) 
		{
			m_CurrentScene->OnRuntimeStart();
		}
	}

	void RuntimeApplication::LoadStartScene() 
	{
		auto project = Project::GetActiveProject();
		if (!project) {
			std::cout << "[RuntimeApplication] No active project set.\n";
			return;
		}

		const auto& config = project->GetConfig();
		
		if (!Project::GetResourceManager()->IsResourceHandleValid(config.StartSceneHandle)) {
			std::cout << "[RuntimeApplication] No StartSceneHandle found in project config.\n";
			return;
		}

		try 
		{
			auto resource = m_ResourceManager->GetResource(config.StartSceneHandle);
			auto scene = std::dynamic_pointer_cast<Scene>(resource);
			if (scene) 
			{
				ChangeScene(scene->GetName() , scene);
				std::cout << "[RuntimeApplication] Loaded Start Scene successfully.\n";
			}
			else 
			{
				std::cerr << "[RuntimeApplication] Resource is not a Scene!\n";
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[RuntimeApplication] Exception while loading Start Scene: " << e.what() << "\n";
		}
	}

}
