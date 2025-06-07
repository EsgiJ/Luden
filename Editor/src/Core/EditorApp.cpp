#include "Core/EditorApp.h"

#include "GUI/ImGuiStyle.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/SceneSettingsPanel.h"
#include "Core/EditorState.h"
#include "Scene/Scene_Zelda.h"
#include <iostream>

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_internal.h>

namespace Luden::Editor {

	EditorApp::EditorApp() {}
	EditorApp::~EditorApp() { Shutdown(); }

	void EditorApp::Init() {
		// Load Editor State
		EditorState loaded = EditorState::Load("editor_state.json");
		EditorStateManager::Get().SetEditorMode(loaded.m_Mode);

		// Create Panels to draw
		m_Panels.clear();
		m_Panels.emplace_back(std::make_unique<SceneHierarchyPanel>());
		m_Panels.emplace_back(std::make_unique<AssetBrowserPanel>());

		for (auto& p : m_Panels) {
			auto it = loaded.m_PanelStates.find(p->GetName());
			p->m_Visible = (it != loaded.m_PanelStates.end()) ? it->second : true;
		}

		m_Window.create(sf::VideoMode(sf::Vector2u(1920, 1080)), "Luden Editor");
		m_Window.setFramerateLimit(60);

		if (!m_ViewportTexture.resize({ 1280, 720 }))
			throw std::runtime_error("Failed to resize viewport texture");
		m_ViewportTextureID = (ImTextureID)(intptr_t)m_ViewportTexture.getTexture().getNativeHandle();

		if (!ImGui::SFML::Init(m_Window))
			return;

		ImGui::StyleColorsDark();
		SetupImGuiStyle(true, 0.9f);
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		m_IsRunning = true;

		ImGuiContext* ctx = ImGui::GetCurrentContext();

		//Initialize Engine
		GameEngine::Initialize(m_Window, ctx, "config/assets.txt", true);
	}

	void EditorApp::Run() {
		while (m_IsRunning && m_Window.isOpen()) {

			float dt = m_DeltaClock.restart().asSeconds();

			if (!EditorStateManager::Get().IsPlayMode())
			{
				HandleInput();
			}

			ImGui::SFML::Update(m_Window, sf::seconds(dt));

			if (EditorStateManager::Get().IsPlayMode()) {
				GameEngine::Get().ProcessInput();
				GameEngine::Get().Update(dt);
			}

			RenderDockSpace();
			RenderModeToolbar();
			for (auto& panel : m_Panels) panel->Render();
			Render();

			ImGui::SFML::Render(m_Window);
			m_Window.display();
		}
	}

	void EditorApp::Shutdown() {
		// Save Current Panel Visibilities
		EditorState& state = EditorStateManager::Get().GetState();
		state.m_Mode = EditorStateManager::Get().GetEditorMode();
		for (auto& p : m_Panels)
			state.m_PanelStates[p->GetName()] = p->m_Visible;
		state.Save("editor_state.json");

		ImGui::SFML::Shutdown();
		if (m_Window.isOpen()) m_Window.close();
	}

	void EditorApp::Render() {
		// viewport
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		sf::Vector2u newSize((unsigned int)viewportPanelSize.x, (unsigned int)viewportPanelSize.y);

		if (m_ViewportTexture.getSize() != newSize && newSize.x > 0 && newSize.y > 0)
		{
			m_ViewportTexture = sf::RenderTexture(newSize);
			m_ViewportTextureID = (ImTextureID)(intptr_t)m_ViewportTexture.getTexture().getNativeHandle();
		}

		GameEngine::Get().Render(m_ViewportTexture);

		float texAspect = static_cast<float>(m_ViewportTexture.getSize().x) / m_ViewportTexture.getSize().y;
		float panelAspect = viewportPanelSize.x / viewportPanelSize.y;
		ImVec2 imageSize = viewportPanelSize;
		if (panelAspect > texAspect) {
			imageSize.x = viewportPanelSize.y * texAspect;
		}
		else if (panelAspect < texAspect) {
			imageSize.y = viewportPanelSize.x / texAspect;
		}

		ImGui::SetCursorPosX((viewportPanelSize.x - imageSize.x) / 2.0f);
		ImGui::SetCursorPosY((viewportPanelSize.y - imageSize.y) / 2.0f);
		ImGui::Image(m_ViewportTextureID, imageSize, { 0, 1 }, { 1, 0 });

		ImGui::End();
	}

	void EditorApp::RenderDockSpace() {
		// Menu Bar
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save All")) { /* Save scene/assets logic */ }
				if (ImGui::MenuItem("Exit")) { m_IsRunning = false; }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				for (auto& panel : m_Panels)
				{
					bool* visible = &panel->m_Visible;
					ImGui::MenuItem(panel->GetName().c_str(), nullptr, visible);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		// Toolbar
		ImGui::SetNextWindowPos(ImVec2(0, 20)); 
		ImGui::SetNextWindowSize(ImVec2((float)m_Window.getSize().x, 40));
		ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;

		if (ImGui::Begin("Toolbar", nullptr, toolbarFlags))
		{
			RenderModeToolbar();
		}
		ImGui::End();

		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		auto* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(0, 60));
		ImGui::SetNextWindowSize(vp->WorkSize);
		ImGui::SetNextWindowViewport(vp->ID);
		flags |= ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("#DockSpace", nullptr, flags);
		ImGui::PopStyleVar(2);

		ImGuiID dock_id = ImGui::GetID("EditorDockSpace");
		ImGui::DockSpace(dock_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

		if (!m_LayoutBuilt) {
			ImGui::DockBuilderRemoveNode(dock_id);               // clear any previous layout
			ImGui::DockBuilderAddNode(dock_id, ImGuiDockNodeFlags_None);
			ImGui::DockBuilderSetNodeSize(dock_id, ImGui::GetIO().DisplaySize);

			// Split main dock into sections
			ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.20f, nullptr, &dock_id);
			ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.25f, nullptr, &dock_id);
			ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Down, 0.25f, nullptr, &dock_id);
			ImGuiID dock_center = dock_id;

			// Optional: right bölümü de ikiye böl (üst: Inspector, alt: Console)
			ImGuiID dock_right_top = ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Up, 0.6f, nullptr, &dock_right);
			ImGuiID dock_right_bottom = dock_right;

			// Dock panels to regions
			ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_left);
			ImGui::DockBuilderDockWindow("Assets", dock_left);
			ImGui::DockBuilderDockWindow("Inspector", dock_right_top);
			ImGui::DockBuilderDockWindow("Console", dock_right_bottom);
			ImGui::DockBuilderDockWindow("Profiler", dock_bottom);
			ImGui::DockBuilderDockWindow("Scene Settings", dock_bottom);
			ImGui::DockBuilderDockWindow("Viewport", dock_center);

			ImGui::DockBuilderFinish(dock_id);
			m_LayoutBuilt = true;
		}

		ImGui::End();
	}


	void EditorApp::RenderModeToolbar()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		float buttonWidth = 60.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x; 
		float totalWidth = buttonWidth * 3 + spacing * 2;

		float startX = (windowSize.x - totalWidth) / 2.0f;
		ImGui::SetCursorPosX(startX);

		if (ImGui::Button("Edit", ImVec2(buttonWidth, 0))) {
			EditorStateManager::Get().SetEditorMode(EditorMode::Edit);
			GameEngine::Get().GetCurrentScene()->SetPaused(true);
		}

		ImGui::SameLine();
		if (ImGui::Button("Play", ImVec2(buttonWidth, 0))) {
			EditorStateManager::Get().SetEditorMode(EditorMode::Play);
			GameEngine::Get().GetCurrentScene()->SetPaused(false);
		}

		ImGui::SameLine();
		if (ImGui::Button("Pause", ImVec2(buttonWidth, 0))) {
			EditorStateManager::Get().SetEditorMode(EditorMode::Pause);
			GameEngine::Get().GetCurrentScene()->SetPaused(true);
		}
	}


	void EditorApp::HandleInput() {
		while (auto evt = m_Window.pollEvent()) {
			ImGui::SFML::ProcessEvent(m_Window, *evt);

			if (evt->is<sf::Event::Closed>())
				m_IsRunning = false;
		}
	}
} 
