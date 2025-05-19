#include "Core/EditorApp.h"

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

		//Initialize Engine
		GameEngine::Initialize("config/assets.txt", true);

		m_Window.create(sf::VideoMode(sf::Vector2u(1280, 720)), "Luden Editor");
		m_Window.setFramerateLimit(60);
		if (!m_ViewportTexture.resize({ 1280, 720 }))
			throw std::runtime_error("Failed to resize viewport texture");
		m_ViewportTextureID = (ImTextureID)(intptr_t)m_ViewportTexture.getTexture().getNativeHandle();

		if (!ImGui::SFML::Init(m_Window))
			return;

		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		m_IsRunning = true;
	}

	void EditorApp::Run() {
		std::vector<sf::Event> sfEvents;
		while (m_IsRunning && m_Window.isOpen()) {
			sfEvents.clear();
			while (const std::optional<sf::Event> event = m_Window.pollEvent()) {
				if (event.has_value())
				{
					sfEvents.emplace_back(event.value());
					ImGui::SFML::ProcessEvent(m_Window, event.value());
				}

				if (event->is<sf::Event::Closed>())
					m_IsRunning = false;
			}

			float dt = m_DeltaClock.restart().asSeconds();
			ImGui::SFML::Update(m_Window, sf::seconds(dt));

			if (EditorStateManager::Get().IsPlayMode()) {
				GameEngine::Get().ProcessInput(sfEvents, ImGui::GetIO().WantCaptureMouse);
				GameEngine::Get().Update(dt);
				GameEngine::Get().Render(m_ViewportTexture);
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
		GameEngine::Shutdown();
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


		ImGui::Image(m_ViewportTextureID, viewportPanelSize, {0, 1}, {1, 0});

		ImGui::End();
	}

	void EditorApp::RenderDockSpace() {
		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		auto* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(vp->WorkPos);
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
			ImGuiIO& io = ImGui::GetIO();
			ImGuiID dock_id = ImGui::GetID("EditorDockSpace");
			ImGui::DockBuilderRemoveNode(dock_id);
			ImGui::DockBuilderAddNode(dock_id, ImGuiDockNodeFlags_None);
			ImGui::DockBuilderSetNodeSize(dock_id, io.DisplaySize);

			ImGuiID left = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.20f, nullptr, &dock_id);
			ImGuiID bottom = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Down, 0.20f, nullptr, &dock_id);
			ImGuiID right = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.20f, nullptr, &dock_id);

			ImGui::DockBuilderDockWindow("Scene Hierarchy", left);
			ImGui::DockBuilderDockWindow("Assets", left);

			ImGui::DockBuilderFinish(dock_id);
			m_LayoutBuilt = true;
		}

		ImGui::End();
	}

	void EditorApp::RenderModeToolbar()
	{
		if (ImGui::Button("Edit")) {
			EditorStateManager::Get().SetEditorMode(EditorMode::Edit);
			GameEngine::Get().GetCurrentScene()->SetPaused(true);
		}

		ImGui::SameLine();
		if (ImGui::Button("Play")) {
			EditorStateManager::Get().SetEditorMode(EditorMode::Play);
			GameEngine::Get().GetCurrentScene()->SetPaused(false);
		}

		ImGui::SameLine();
		if (ImGui::Button("Pause")) {
			EditorStateManager::Get().SetEditorMode(EditorMode::Pause);
			GameEngine::Get().GetCurrentScene()->SetPaused(true);
		}

		ImGui::Text("Current Mode: %s",
			EditorStateManager::Get().IsEditMode() ? "Edit" :
			EditorStateManager::Get().IsPlayMode() ? "Play" :
			EditorStateManager::Get().IsPaused() ? "Pause" : "Unknown");
	}

	void EditorApp::HandleInput() {
		while (auto evt = m_Window.pollEvent()) {
			ImGui::SFML::ProcessEvent(m_Window, *evt);
			if (evt->is<sf::Event::Closed>())
				m_IsRunning = false;
		}
	}

} // namespace Luden::Editor
