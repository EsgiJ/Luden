#include "Core/RuntimeApplication.h"
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
#include "Scene/Scene.h"
#include "Resource/ResourceManager.h"

#include <iostream>

namespace Luden {

	RuntimeApplication::RuntimeApplication(const ApplicationSpecification& spec)
		: m_Specification(std::move(spec))
	{

		sf::VideoMode vm({ m_Specification.WindowWidth, m_Specification.WindowHeight });
		m_Window = std::make_unique<sf::RenderWindow>(vm, m_Specification.Name);
	}

	RuntimeApplication::~RuntimeApplication() 
	{
		OnShutdown();
	}

	void RuntimeApplication ::OnInit() 
	{
		std::cout << "[RuntimeApplication] Initializing...\n";

		OpenProject();
		auto project = Project::GetActiveProject();
		Project::SetActiveRuntime(project, m_ResourcePack);

		m_ResourceManager = Project::GetRuntimeResourceManager();
		if (!m_ResourceManager) 
		{
			std::cerr << "[RuntimeApplication] ResourceManager not initialized!\n";
			return;
		}
		
		LoadScene(project->GetConfig().StartSceneHandle);
	}

	void RuntimeApplication::OnShutdown() 
	{
		if (m_CurrentScene) 
		{
			//m_CurrentScene->OnRuntimeStop();
			m_CurrentScene.reset();
		}
		m_ResourceManager.reset();

		if (m_Window && m_Window->isOpen())
			m_Window->close();
	}

	void RuntimeApplication::OnUpdate(TimeStep ts) 
	{

		if (!m_CurrentScene) return;

		//m_CurrentScene->OnUpdateRuntime(ts);

		if (m_Window) 
		{
			m_Window->clear();
			//m_CurrentScene->OnRenderRuntime(*m_Window);
			m_Window->display();
		}
	}

	void RuntimeApplication::RenderTo(sf::RenderTarget& target) 
	{
		if (m_CurrentScene) {
			//m_CurrentScene->OnRenderRuntime(target);
		}
	}

	void RuntimeApplication::OpenProject()
	{
		std::shared_ptr<Project> project = std::make_shared<Project>();
		ProjectSerializer serializer(project);
		serializer.DeserializeRuntime(std::filesystem::path(m_Specification.m_ProjectPath) / project->GetConfig().ResourceDirectory / "Project.ldat");

		// Load asset pack
		m_ResourcePack = ResourcePack::Load(std::filesystem::path(m_Specification.m_ProjectPath) / "ResourcePack.lrp");
		Project::SetActiveRuntime(project, m_ResourcePack);

		LoadScene(project->GetConfig().StartSceneHandle);
	}

	void RuntimeApplication::ChangeScene(const std::string& name, std::shared_ptr<Scene> scene, bool endCurrent) 
	{
		if (endCurrent && m_CurrentScene)
		{
			//m_CurrentScene->OnRuntimeStop();
		}

		m_CurrentScene = scene;
		m_CurrentSceneName = name;

		if (m_CurrentScene) 
		{
			//m_CurrentScene->OnRuntimeStart();
		}
	}

	void RuntimeApplication::LoadScene(ResourceHandle handle) {
		if (!m_ResourceManager) {
			std::cerr << "[RuntimeApplication] ResourceManager not initialized!\n";
			return;
		}

		if (!Project::GetResourceManager()->IsResourceHandleValid(handle)) {
			std::cerr << "[RuntimeApplication] Invalid StartScene handle!\n";
			return;
		}
		auto scene = ResourceManager::GetResource<Scene>(handle);
		if (scene)
		{
			ChangeScene(scene->GetName(), scene);
		}
		else 
		{
			std::cerr << "[RuntimeApplication] StartScene resource is not a Scene!\n";
		}
	}
}
