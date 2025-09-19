#include "Core/EditorApplication.h"

#include "Utils/EditorColors.h"
#include "Tabs/SceneEditorTab.h"
#include "IO/FileSystem.h"
#include "Resource/ResourceManager.h"
#include "Scene/SceneSerializer.h"
#include "Utils/EditorResources.h"


#include <filesystem>

#include <iostream>

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_internal.h>

namespace Luden 
{

	EditorApplication::EditorApplication() {}
	EditorApplication::~EditorApplication() { Shutdown(); }

	void EditorApplication::Init() 
	{
		EditorResources::LoadEditorResources(Project::GetProjectDirectory());


		// Editor window
		m_Window.create(sf::VideoMode(sf::Vector2u(1920, 1080)), "Luden Editor", sf::Style::None);
		m_Window.setFramerateLimit(60);

		if (!ImGui::SFML::Init(m_Window))
			return;

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Start RuntimeApplication headless
		ApplicationSpecification spec;
		spec.Name = "Luden Runtime (Editor)";
		spec.WindowWidth = 1280;
		spec.WindowHeight = 720;
		spec.Headless = true;
		spec.EnableImGui = false;
		spec.m_ProjectPath = "C:\\GameProjects\\Luden\\MyGame";
		m_RuntimeApp = std::make_unique<Luden::RuntimeApplication>(std::move(spec));
		m_RuntimeApp->OnInit();
	}

	void EditorApplication::Run()
	{
		TimeStep timestep;
		OnUpdate(timestep);
	}

	void EditorApplication::Shutdown()
	{
		ImGui::SFML::Shutdown();
		if (m_Window.isOpen()) 
			m_Window.close();
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
						set->OnSceneStop();
					}
				}
				m_FocusedTab = tab;
			}
		}

		if (m_FocusedTab) 
		{
			m_FocusedTab->OnUpdate(timestep);
		}
	}

	void EditorApplication::OnEvent(const std::optional<sf::Event>& evt)
	{
		if (m_FocusedTab) 
		{
			//m_FocusedTab->OnEvent(event);
		}
	}
	void EditorApplication::OnImGuiRender()
	{
		if (BeginMainDockspace()) 
		{
			RenderContent();
			InitializeMainDockspace();
		}
		ImGui::End();


		// closing tabs
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

	void EditorApplication::CreateNewProject(const std::filesystem::path& path, NewProjectType type)
	{
		if (!FileSystem::Exists(path)) 
		{
			FileSystem::CreateDir(path);
		}

		std::filesystem::path projectTemplate = EditorResources::ProjectTemplate;
		switch (type) 
		{
		case NewProjectType::EMPTY:	projectTemplate = EditorResources::ProjectTemplate;  break;
		}

		std::filesystem::copy(projectTemplate, path,
			std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

		LoadProject(path / "project.luden");
	}

	void EditorApplication::LoadProject(const std::filesystem::path& path)
	{

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
		if (ext == ".lscn") {
			tab = std::static_pointer_cast<EditorTab>(std::make_shared<SceneEditorTab>(path));
		}

		if (tab) 
		{
			tab->DockTo(m_MainDockspaceID);
			m_EditorTabs.push_back(tab);
			m_FocusedTab = tab;
			ImGui::FocusWindow(ImGui::FindWindowByName(tab->GetWindowName().c_str()));
		}
	}

	bool EditorApplication::BeginMainDockspace()
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		constexpr ImGuiWindowFlags main_window_flags = ImGuiWindowFlags_NoDocking //| ImGuiWindowFlags_MenuBar
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::SetNextWindowClass(&m_MainDockspaceClass);
		ImGui::Begin("MainDockSpace", nullptr, main_window_flags);

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
	}

	void EditorApplication::SaveProject()
	{

	}
	void EditorApplication::ReloadProject()
	{

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

	void EditorApplication::UpdateWindowTitle()
	{

	}

	void EditorApplication::OnOverlayRender()
	{

	}
}
