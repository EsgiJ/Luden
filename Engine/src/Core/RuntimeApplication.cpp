#include "Core/RuntimeApplication.h"
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Resource/ResourceManager.h"
#include "NativeScript/NativeScriptModuleLoader.h"
#include "Core/Config.h"
#include "IO/FileSystem.h"
#include "SFML/Graphics/Sprite.hpp"

#include <imgui-SFML.h>

#include <iostream>

namespace Luden {

	RuntimeApplication::RuntimeApplication(const ApplicationSpecification& spec)
		: m_Specification(spec)
	{
	}

	RuntimeApplication::~RuntimeApplication()
	{
	}

	void RuntimeApplication::Init()
	{
		sf::VideoMode vm(sf::Vector2u(m_Specification.WindowWidth, m_Specification.WindowHeight));
		m_Window = std::make_unique<sf::RenderWindow>(
			sf::VideoMode{ {m_Specification.WindowWidth, m_Specification.WindowHeight} },
			m_Specification.Name,
			sf::Style::Default  
		);
		if (m_Specification.VSync)
			m_Window->setVerticalSyncEnabled(true);
		else
			m_Window->setFramerateLimit(0);

		m_RenderTexture = std::make_shared<sf::RenderTexture>();
		if (!m_RenderTexture->resize({ m_Specification.WindowWidth, m_Specification.WindowHeight }))
		{
			std::cerr << "[RuntimeApplication] ERROR: Failed to create RenderTexture!\n";
			return;
		}

		if (m_Specification.VSync)
			m_Window->setVerticalSyncEnabled(true);

		auto project = Project::GetActiveProject();
		if (!project)
		{
			std::cerr << "[RuntimeApplication] No active project!\n";
			return;
		}

		m_ResourceManager = Project::GetEditorResourceManager();
		if (!m_ResourceManager)
		{
			std::cerr << "[RuntimeApplication] ResourceManager not initialized!\n";
			return;
		}

		m_ResourceManager->LoadResourceRegistry();
		m_ResourceManager->ReloadResources();

		m_NativeScriptModuleLoader = std::make_unique<NativeScriptModuleLoader>();
		std::filesystem::path modulePath = Config::GetGameModulePath();

		if (m_NativeScriptModuleLoader->LoadModule(modulePath))
		{
			m_NativeScriptModuleLoader->GetModule()->RegisterScripts(m_ResourceManager.get());
		}
		else
		{
			std::cerr << "[RuntimeApplication] Failed to load native script module!\n";
		}

		LoadScene(project->GetConfig().StartSceneHandle);
	}

	void RuntimeApplication::Run()
	{
		m_Clock.restart();

		while (m_Window && m_Window->isOpen() && m_Running)
		{
			while (const std::optional event = m_Window->pollEvent())
			{
				if (event->is<sf::Event::Closed>())
				{
					m_Window->close();
				}

				if (m_CurrentScene)
				{
					m_CurrentScene->OnEvent(event);
				}
			}

			sf::Time dt = m_Clock.restart();
			TimeStep timestep(dt.asSeconds());
			OnUpdate(timestep);

			m_Window->clear(sf::Color(30, 30, 30));

			if (m_CurrentScene)
			{
				m_RenderTexture->clear();
				m_CurrentScene->OnUpdateRuntime(timestep, m_RenderTexture);
				m_RenderTexture->display();

				sf::Vector2u windowSize = m_Window->getSize();
				sf::Vector2u textureSize = m_RenderTexture->getSize();

				sf::View windowView(sf::FloatRect(
					{ 0.f, 0.f },
					{ static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) }
				));
				m_Window->setView(windowView);

				sf::Sprite sprite(m_RenderTexture->getTexture());

				float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
				float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
				sprite.setScale({ scaleX, scaleY });

				m_Window->draw(sprite);
			}
			std::cout << "FPS: " << 1.f / dt.asSeconds() << "\r" << std::flush;

			m_Window->display();
		}

		Shutdown();
	}

	void RuntimeApplication::Shutdown()
	{
		if (Project::GetResourceManager())
		{
			Project::GetResourceManager()->Shutdown();
		}

		ImGui::SFML::Shutdown();

		if (m_Window && m_Window->isOpen())
			m_Window->close();
	}

	void RuntimeApplication::OnUpdate(TimeStep ts)
	{
		// Can be overridden by project app
	}

	void RuntimeApplication::OpenProject()
	{
		std::shared_ptr<Project> project = std::make_shared<Project>();
		ProjectSerializer serializer(project);

		std::filesystem::path projectPath = m_Specification.ProjectPath;

		if (!FileSystem::Exists(projectPath))
		{
			std::cerr << "[RuntimeApplication] Project file not found: " << projectPath << "\n";
			return;
		}

		if (!serializer.Deserialize(projectPath))
		{
			std::cerr << "[RuntimeApplication] Failed to deserialize project!\n";
			return;
		}

		Project::SetActive(project);
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

			if (m_Window)
			{
				sf::Vector2u windowSize = m_Window->getSize();
				m_CurrentScene->SetViewportSize(m_Specification.WindowWidth, m_Specification.WindowHeight);
			}
			else
			{
				std::cout << "[RuntimeApplication::ChangeScene] WARNING: Window is null!\n";
			}

			m_CurrentScene->OnRuntimeStart();
		}
		else
		{
			std::cout << "[RuntimeApplication::ChangeScene] ERROR: Scene is null after assignment!\n";
		}
	}

	void RuntimeApplication::LoadScene(ResourceHandle handle)
	{
		if (!m_ResourceManager)
		{
			std::cerr << "[RuntimeApplication::LoadScene] ERROR: ResourceManager not initialized!\n";
			return;
		}

		if (!m_ResourceManager->IsResourceHandleValid(handle))
		{
			std::cerr << "[RuntimeApplication::LoadScene] ERROR: Invalid StartScene handle: " << handle << "\n";

			auto allScenes = m_ResourceManager->GetAllResourcesWithType(ResourceType::Scene);
			std::cout << "[RuntimeApplication::LoadScene] Available scenes in registry: " << allScenes.size() << "\n";
			for (auto sceneHandle : allScenes)
			{
				auto metadata = m_ResourceManager->GetMetadata(sceneHandle);
				std::cout << "  - Handle: " << sceneHandle << ", Path: " << metadata.FilePath << "\n";
			}
			return;
		}

		auto scene = ResourceManager::GetResource<Scene>(handle);

		if (scene)
		{
			ChangeScene(scene->GetName(), scene);
		}
		else
		{
			std::cerr << "[RuntimeApplication::LoadScene] ERROR: Failed to get Scene resource!\n";
			std::cerr << "[RuntimeApplication::LoadScene] Handle: " << handle << "\n";

			auto metadata = m_ResourceManager->GetMetadata(handle);
			std::cout << "[RuntimeApplication::LoadScene] Metadata - FilePath: " << metadata.FilePath
				<< ", Type: " << (int)metadata.Type << "\n";
		}
	}
}