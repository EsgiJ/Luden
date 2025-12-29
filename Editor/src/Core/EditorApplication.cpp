#include "Core/EditorApplication.h"

#include "Utils/EditorColors.h"
#include "Tabs/SceneEditorTab.h"
#include "IO/FileSystem.h"
#include "Resource/ResourceManager.h"
#include "Project/ProjectSerializer.h"
#include "Scene/SceneSerializer.h"
#include "Utils/EditorResources.h"
#include "Utils/ImGuiStyle.h"
#include "NativeScript/NativeScriptModuleLoader.h"
#include "Core/Config.h"
#include <Tabs/Animation2DEditorTab.h>

#include <filesystem>
#include <iostream>

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_internal.h>
#include "Tabs/SpriteEditorTab.h"
#include "Project/ProjectGenerator.h"


namespace Luden 
{
	EditorApplication::EditorApplication() {}
	EditorApplication::~EditorApplication() {}

	void EditorApplication::Init() 
	{
		// Editor window
		m_Window.create(sf::VideoMode(sf::Vector2u(1920, 1080)), "Luden Editor", sf::Style::Titlebar);
		m_Window.setFramerateLimit(60);

		if (!ImGui::SFML::Init(m_Window))
			std::cerr << "Window initialization error";

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		EditorResources::Init();

		// Font config
		io.Fonts->Clear();
		ImFont* mainFont = io.Fonts->AddFontFromFileTTF("Resources/fonts/JetBrainsMono-Medium.ttf", 18.0f);
		ImFontConfig fontConfig;
		fontConfig.MergeMode = true;
		static const ImWchar iconRanges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
		io.Fonts->AddFontFromFileTTF("Resources/fonts/fa-solid-900.ttf", 18.0f, &fontConfig, iconRanges);
		io.Fonts->Build();
		if (!ImGui::SFML::UpdateFontTexture())
		{
			//TODO: ASSERT Unable to update the font texture
		}
		SetupImGuiStyle();
	}

	void EditorApplication::Run()
	{
		sf::Clock clock;
		bool running = true;

		while (m_Window.isOpen() && running)
		{
			while (const std::optional event = m_Window.pollEvent())
			{
				ImGui::SFML::ProcessEvent(m_Window, *event);

				if (event->is<sf::Event::Closed>())
					m_Window.close();

				OnEvent(event);
			}

			sf::Time dt = clock.restart();
			TimeStep timestep(dt.asSeconds());

			ImGui::SFML::Update(m_Window, dt);

			m_Window.clear(sf::Color(30, 30, 30));

			OnUpdate(timestep);

			OnImGuiRender();

			ImGui::SFML::Render(m_Window);

			m_Window.display();
		}

		Shutdown();
	}


	void EditorApplication::Shutdown()
	{
		m_EditorTabs.clear();

		ImGui::SFML::Shutdown();
		if (m_Window.isOpen()) 
			m_Window.close();

		if (Project::GetResourceManager())
		{
			Project::GetResourceManager()->Shutdown();
		}
	}

	void EditorApplication::OnUpdate(TimeStep timestep)
	{
		for (const auto& tab : m_EditorTabs) 
		{
			if (tab->IsFocused()) 
			{
				if (m_FocusedTab != tab) 
				{
					if (auto set = std::dynamic_pointer_cast<SceneEditorTab>(m_FocusedTab)) 
					{
						if (set->GetSceneState() == SceneEditorTab::SceneState::Play)
						{
							set->OnSceneStop();
						}
					}
				}
				m_FocusedTab = tab;
			}
		}

		if (m_FocusedTab) 
		{
			m_FocusedTab->OnUpdate(timestep);
		}

		if (Project::GetActiveProject() && m_NativeScriptModuleLoader)
		{
			HotReloadNativeScripts();
		}
	}

	void EditorApplication::OnEvent(const std::optional<sf::Event>& evt)
	{
		if (m_FocusedTab) 
		{
			m_FocusedTab->OnEvent(evt);
		}
	}

	void EditorApplication::OnImGuiRender()
	{
		RenderTitleBar();
		RenderMenuBar();
		if (BeginMainDockspace()) 
		{
			InitializeMainDockspace();
			RenderContent();
		}
		ImGui::End();


		// Closing tabs
		m_EditorTabs.erase(
			std::remove_if(m_EditorTabs.begin(), m_EditorTabs.end(),
				[](const std::shared_ptr<EditorTab>& tab) {
					if (tab->ShouldClose()) 
					{
						return true;
					}
					return false;
				}
			),
			m_EditorTabs.end()
		);

		if (m_EditorTabs.size() == 0 && m_OpenResourceRequests.size() == 0) {

			RequestOpenResource("Home");
		}
	}

	void EditorApplication::RequestOpenResource(const std::filesystem::path& path)
	{
		// do not open new if its already open
		for (const auto& tab : m_EditorTabs) 
		{
			if (tab->GetName() == path) 
			{
				ImGui::FocusWindow(ImGui::FindWindowByName(tab->GetWindowName().c_str()));
				return;
			}
		}

		m_OpenResourceRequests.push_back(path);
	}

	void EditorApplication::HotReloadNativeScripts()
	{
		std::filesystem::path modulePath = Config::GetGameModulePath();
		auto currentWriteTime = FileSystem::GetLastWriteTime(modulePath);

		if (currentWriteTime > m_LastModuleWriteTime)
		{
			std::cout << "DLL changed, hot reloading..." << std::endl;

			if (!m_FocusedTab)
				return;

			if (auto sceneTab = std::dynamic_pointer_cast<SceneEditorTab>(m_FocusedTab))
			{
				std::shared_ptr<Scene> activeScene = sceneTab->GetActiveScene();

				if (activeScene == nullptr)
					return;

				bool wasPlaying = sceneTab->GetSceneState() == SceneEditorTab::SceneState::Play;

				if (wasPlaying)
				{
					sceneTab->OnSceneStop();
				}
				else
				{
					for (auto& entity : activeScene->GetEntityManager().GetEntities())
					{
						if (entity.Has<NativeScriptComponent>())
						{
							auto& nsc = entity.Get<NativeScriptComponent>();
							if (nsc.Instance != nullptr)
							{
								nsc.DestroyInstance();
							}
						}
					}
				}

				if (m_NativeScriptModuleLoader->ReloadModule())
				{
					if (!m_NativeScriptModuleLoader->GetModule())
					{
						std::cerr << "Hot reload failed: Module is null after reload!" << std::endl;
						return;
					}

					auto resourceManager = Project::GetResourceManager();
					if (!resourceManager)
					{
						std::cerr << "Hot reload failed: ResourceManager is null!" << std::endl;
						return;
					}

					m_NativeScriptModuleLoader->GetModule()->RegisterScripts(resourceManager.get());
					m_LastModuleWriteTime = currentWriteTime;
					std::cout << "Hot reload successful!" << std::endl;					

					if (wasPlaying)
					{
						sceneTab->OnScenePlay();
					}
				}
				else
				{
					std::cerr << "Hot reload failed: Module reload returned false!" << std::endl;
				}
			}
		}
	}

	void CreateNewScene()
	{

	}

	void EditorApplication::CreateNewScene()
	{
		std::filesystem::path newPath = FileSystem::SaveFileDialog({ { "Luden Scene (*.lscene)", "lscene" } });
		if (newPath.empty())
			return;

		std::filesystem::path relative = Project::GetEditorResourceManager()->GetRelativePath(newPath);

		if (FileSystem::Exists(newPath))
		{
			//TODO: Override logic
			bool overrideConfirmed = true;
			if (!overrideConfirmed)
				return;
		}

		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer(newScene);
		serializer.Serialize(newPath.string());
		RequestOpenResource(relative);
	}

	void EditorApplication::ExitEditor()
	{
		Shutdown();
	}

	void EditorApplication::ProcessOpenResourceRequests()
	{
		if (!m_MainDockspaceInitialized) 
			return;

		if (m_OpenResourceRequests.empty())
			return;

		for (auto& path : m_OpenResourceRequests)
			OpenResource(path);

		m_OpenResourceRequests.clear();
	}

	void EditorApplication::OpenResource(const std::filesystem::path& path)
	{
		std::string ext = path.extension().string();
		std::string filename = path.filename().string();

		std::shared_ptr<EditorTab> tab = nullptr;

		if (ext == ".lscene")
		{
			tab = std::static_pointer_cast<EditorTab>(std::make_shared<SceneEditorTab>(path));
			tab->SetEditorContext(this);
		}
		else if (ext == ".lanim")
		{
			tab = std::static_pointer_cast<EditorTab>(std::make_shared<AnimationEditorTab>(path));
		}
		else if (ext == ".lsprite")
		{
			tab = std::static_pointer_cast<EditorTab>(std::make_shared<SpriteEditorTab>(path));
		}

		if (tab) 
		{
			tab->DockTo(m_MainDockspaceID);
			m_EditorTabs.push_back(tab);
			m_FocusedTab = tab;
			tab->SetEditorContext(this);
			ImGui::FocusWindow(ImGui::FindWindowByName(tab->GetWindowName().c_str()));
		}
	}

	bool EditorApplication::BeginMainDockspace()
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(
			ImVec2(viewport->WorkPos.x, viewport->WorkPos.y)
		);
		ImGui::SetNextWindowSize(
			ImVec2(viewport->WorkSize.x, viewport->WorkSize.y)
		);
		ImGui::SetNextWindowViewport(viewport->ID);
		constexpr ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoDocking //| ImGuiWindowFlags_MenuBar
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::SetNextWindowClass(&m_MainDockspaceClass);
		ImGui::Begin("MainDockSpace", nullptr, mainWindowFlags);

		ImGui::PopStyleVar(3);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::EditorTabPadding);
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) 
		{
			m_MainDockspaceID = ImGui::GetID("MainDockSpaceID");
			ImGui::SetNextWindowClass(&m_MainDockspaceClass);
			ImGui::DockSpace(m_MainDockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoDockingSplit);
		}
		ImGui::PopStyleVar();

		return true;
	}

	void EditorApplication::RenderTitleBar()
	{
		const float desiredHeight = 30.0f;

		float fontHeight = ImGui::GetTextLineHeight();
		float newFramePaddingY = (desiredHeight - fontHeight) / 2.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
			ImVec2(ImGui::GetStyle().FramePadding.x, newFramePaddingY)
		);

		if (ImGui::BeginMainMenuBar())
		{
			ImGui::PopStyleVar();

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			float actualMenuHeight = ImGui::GetFrameHeight();

			const float logoSize = 30.0f;
			float logoOffset = (actualMenuHeight - logoSize) / 2.0f;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + logoOffset);

			ImGui::Image(EditorResources::BannerIcon->GetTexture().getNativeHandle(), ImVec2(logoSize, logoSize));
			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (logoSize - ImGui::GetTextLineHeight()) / 2.0f);
			ImGui::Text("Luden Editor");
			ImGui::SameLine(0.0f, 15.0f);

			ImGui::SetCursorPosY(0.0f);

			if (ImGui::BeginMenu(ICON_FA_FILE " File"))
			{
				if (ImGui::MenuItem(ICON_FA_FOLDER_PLUS " New Project"))
				{
					m_ShowNewProjectPopup = true;
				}
				if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Load Project"))
				{
					m_ShowLoadProjectPopup = true;
				}
				ImGui::Separator();

				if (ImGui::MenuItem(ICON_FA_FILE " New Scene"))
				{
					CreateNewScene();
				}
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " Save Scene"))
				{
					SaveActiveScene();
				}
				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_RIGHT_FROM_BRACKET " Exit"))
				{
					ExitEditor();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_PEN_TO_SQUARE " Edit"))
			{
				if (ImGui::MenuItem(" Undo"))
				{
					// Undo Logic
				}
				ImGui::EndMenu();
			}

			const float controlsWidth = 120.0f;
			float dragRegionWidth = viewport->Size.x - ImGui::GetCursorPosX() - controlsWidth;

			ImGui::InvisibleButton("TitleBarDrag", ImVec2(dragRegionWidth, actualMenuHeight));

			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				sf::Vector2i currentPos = m_Window.getPosition();
				sf::Vector2i delta = sf::Vector2i((int)ImGui::GetIO().MouseDelta.x, (int)ImGui::GetIO().MouseDelta.y);
				sf::Vector2i newPos = currentPos + delta;
				m_Window.setPosition(newPos);
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(viewport->Size.x - controlsWidth + 5.0f);

			if (ImGui::Button(ICON_FA_MINUS, ImVec2(35, actualMenuHeight)))
			{
				/* minimize */
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_XMARK, ImVec2(35, actualMenuHeight)))
			{
				m_Window.close();
			}

			ImGui::EndMainMenuBar();
		}
		else {
			ImGui::PopStyleVar();
		}

		ShowNewProjectPopup();
		ShowLoadProjectPopup();
	}

	void EditorApplication::RenderContent()
	{
		// open requested assets
		ProcessOpenResourceRequests();

		for (const auto& tab : m_EditorTabs) 
		{
			ImGui::SetNextWindowClass(&m_MainDockspaceClass);
			tab->OnImGuiRender();
		}
	}
	void EditorApplication::InitializeMainDockspace()
	{
		if (m_MainDockspaceInitialized) 
			return;


		m_MainDockspaceID = ImGui::GetID("MainDockSpaceID");
		ImGui::DockBuilderRemoveNode(m_MainDockspaceID);
		ImGui::DockBuilderAddNode(m_MainDockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_MainDockspaceID, ImGui::GetMainViewport()->Size);
		ImGui::DockBuilderFinish(m_MainDockspaceID);

		m_MainDockspaceClass.ClassId = ImGui::GetID("MainDockspaceClass");
		m_MainDockspaceClass.DockingAllowUnclassed = false;

		m_MainDockspaceInitialized = true;

		if (Project::GetActiveProject())
		{
			std::filesystem::path startScene = Project::GetActiveProject()->GetConfig().StartScene;
			if (!startScene.empty() && FileSystem::Exists(startScene))
			{
				RequestOpenResource(startScene);
			}

			// Load Native Script Module
			m_NativeScriptModuleLoader = std::make_unique<NativeScriptModuleLoader>();
			std::filesystem::path modulePath = Config::GetGameModulePath();

			if (m_NativeScriptModuleLoader->LoadModule(modulePath))
			{
				auto resourceManager = Project::GetResourceManager();
				m_NativeScriptModuleLoader->GetModule()->RegisterScripts(resourceManager.get());

				m_LastModuleWriteTime = FileSystem::GetLastWriteTime(modulePath);
			}
		}
	}

	void EditorApplication::EmptyProject()
	{
		if (Project::GetActiveProject())
			CloseProject();

		std::shared_ptr<Project> project = std::make_shared<Project>();

	}

	void EditorApplication::SaveProject()
	{
		if (!Project::GetActiveProject())
			return; // TODO: 

		std::shared_ptr<Project> activeProject = Project::GetActiveProject();
		ProjectSerializer serializer(activeProject);
		serializer.Serialize(activeProject->GetConfig().ProjectDirectory + "/" + activeProject->GetConfig().ProjectFileName);
	}

	void EditorApplication::ShowNewProjectPopup()
	{
		if (m_ShowNewProjectPopup)
		{
			ImGui::OpenPopup("New Project");
			m_ShowNewProjectPopup = false;
			m_PopupStatusMessage.clear();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

		if (ImGui::BeginPopupModal("New Project", nullptr, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Create a new Luden project");
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Project Name:");
			ImGui::SetNextItemWidth(-1);
			ImGui::InputText("##ProjectName", m_ProjectNameBuffer, sizeof(m_ProjectNameBuffer));

			ImGui::Spacing();

			ImGui::Text("Project Path:");
			ImGui::SetNextItemWidth(-120);
			ImGui::InputText("##ProjectPath", m_ProjectPathBuffer, sizeof(m_ProjectPathBuffer));

			ImGui::SameLine();
			if (ImGui::Button("Browse...", ImVec2(110, 0)))
			{
				std::filesystem::path selectedPath = FileSystem::OpenFolderDialog();
				if (!selectedPath.empty())
				{
					strncpy_s(m_ProjectPathBuffer, selectedPath.string().c_str(), sizeof(m_ProjectPathBuffer));
				}
			}

			ImGui::Spacing();
			ImGui::Spacing();

			if (!m_PopupStatusMessage.empty())
			{
				ImVec4 color = (m_PopupStatusMessage.find("success") != std::string::npos)
					? ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
					: ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

				ImGui::TextColored(color, "%s", m_PopupStatusMessage.c_str());
				ImGui::Spacing();
			}

			ImGui::Separator();

			bool canCreate = strlen(m_ProjectNameBuffer) > 0 && strlen(m_ProjectPathBuffer) > 0;

			ImGui::BeginDisabled(!canCreate);
			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				std::string projectName(m_ProjectNameBuffer);
				std::filesystem::path projectPath(m_ProjectPathBuffer);

				CreateNewProject(projectName, projectPath);

				ImGui::CloseCurrentPopup();

				memset(m_ProjectNameBuffer, 0, sizeof(m_ProjectNameBuffer));
				strcpy_s(m_ProjectNameBuffer, "MyGame");
				memset(m_ProjectPathBuffer, 0, sizeof(m_ProjectPathBuffer));
			}
			ImGui::EndDisabled();

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_PopupStatusMessage.clear();
			}

			ImGui::EndPopup();
		}
	}

	void EditorApplication::ShowLoadProjectPopup()
	{
		if (m_ShowLoadProjectPopup)
		{
			std::filesystem::path projectFile = FileSystem::OpenFileDialog({
				{"Luden Project", "lproject"}
				});

			if (!projectFile.empty())
			{
				CloseAllTabs();
				LoadProject(projectFile);
			}

			m_ShowLoadProjectPopup = false;
		}
	}

	void EditorApplication::CreateNewProject(const std::string& projectName, const std::filesystem::path& path)
	{
		if (ProjectGenerator::CreateProject(projectName, path))
		{
			CloseAllTabs();

			std::filesystem::path projectPath = path / projectName / (projectName + ".lproject");
			LoadProject(projectPath);

			m_PopupStatusMessage = "Project created successfully!";

			LoadProject(projectPath);
		}
		else
		{
			m_PopupStatusMessage = "Failed to create project!";
		}
	}

	void EditorApplication::LoadProject(const std::filesystem::path& path)
	{
		std::shared_ptr<Project> activeProject = std::make_shared<Project>();
		ProjectSerializer serializer(activeProject);

		if (!serializer.Deserialize(path))
		{
			std::cerr << "Failed to load project: " << path << std::endl;
			return;
		}

		Project::SetActive(activeProject);

		std::filesystem::path startScene = (activeProject->GetConfig().ProjectDirectory / activeProject->GetConfig().StartScene);
		
		if (!startScene.empty() && FileSystem::Exists(startScene))
		{
			RequestOpenResource(startScene);
		}

		m_NativeScriptModuleLoader = std::make_unique<NativeScriptModuleLoader>();
		std::filesystem::path modulePath = Config::GetGameModulePath();

		if (m_NativeScriptModuleLoader->LoadModule(modulePath))
		{
			auto resourceManager = Project::GetResourceManager();
			m_NativeScriptModuleLoader->GetModule()->RegisterScripts(resourceManager.get());

			m_LastModuleWriteTime = FileSystem::GetLastWriteTime(modulePath);
		}
	}

	void EditorApplication::CloseProject()
	{
		SaveProject();
	}

	void EditorApplication::CloseAllTabs()
	{
		for (const auto& tab : m_EditorTabs)
		{
			if (auto sceneTab = std::dynamic_pointer_cast<SceneEditorTab>(tab))
			{
				if (sceneTab->GetSceneState() == SceneEditorTab::SceneState::Play)
				{
					sceneTab->OnSceneStop();
				}
			}
		}

		m_EditorTabs.clear();
		m_FocusedTab = nullptr;
	}

	void EditorApplication::ReloadTab()
	{
		for (auto it = m_EditorTabs.begin(); it != m_EditorTabs.end(); ++it) 
		{
			std::shared_ptr<EditorTab> tab = *it;
			if (tab->IsFocused() && !tab->IsDirty()) 
			{
				auto name = tab->GetName();
				m_EditorTabs.erase(it);
				RequestOpenResource(name);
				return;
			}
		}
	}

	bool EditorApplication::OnKeyPressed(const sf::Event::KeyPressed& key)
	{
		return false;
	}

	void EditorApplication::RenderMenuBar()
	{

	}

	void EditorApplication::SaveActiveScene()
	{
		if (m_FocusedTab)
		{
			if (auto sceneTab = std::dynamic_pointer_cast<SceneEditorTab>(m_FocusedTab))
			{
				sceneTab->SaveScene();
			}
		}
	}
}
