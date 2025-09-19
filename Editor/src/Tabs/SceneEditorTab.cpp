#include "Tabs/SceneEditorTab.h"

#include "Utils/EditorColors.h"
#include "IO/FileSystem.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Project/Project.h"

#include <imgui.h>
#include <imgui-SFML.h>

namespace Luden
{
	SceneEditorTab::SceneEditorTab(const std::filesystem::path& name)
		: EditorTab(name), m_ViewportPanelName("Viewport##" + std::to_string(m_TabID))
	{
		m_RenderTexture = std::make_shared<sf::RenderTexture>();
		m_ToolbarPanel.InitValues(m_RenderTexture, m_ViewportHovered);

		std::filesystem::path path(name);
		SetWindowName(path.filename().string());

		LoadScene(path);

		SetPanelsContext();
	}

	SceneEditorTab::~SceneEditorTab() 
	{
		OnSceneStop();
	}

	void SceneEditorTab::OnScenePlay()
	{
		OnScenePause(false);

		SaveScene();

		if (m_ActiveScenePath.empty()) 
			return;

		UUID currentSelectedEntity = m_SceneHierarchyPanel.GetSelectedEntityUUID();

		m_SceneState = SceneState::Play;

		/* Copy Current Editor Scene */ 
		{
			std::shared_ptr<Scene> new_scene = std::make_shared<Scene>();
			SceneSerializer serializer = SceneSerializer(new_scene);
			if (serializer.Deserialize(m_ActiveScenePath.string())) {
				m_ActiveScene = new_scene;
				SetPanelsContext();
			}
		}

		m_SceneHierarchyPanel.SetSelectedEntityWithUUID(currentSelectedEntity);
		ImGui::SetWindowFocus(m_ViewportPanelName.c_str());
	}

	void SceneEditorTab::OnSceneStop() {
		OnScenePause(false);


		UUID currentSelectedEntity = 0;
		if (m_SceneHierarchyPanel.IsSelectedEntityValid()) 
		{
			currentSelectedEntity = m_SceneHierarchyPanel.GetSelectedEntityUUID();
		}

		m_SceneState = SceneState::Edit;
		m_ActiveScene = m_EditorScene;
		SetPanelsContext();

		m_SceneHierarchyPanel.SetSelectedEntityWithUUID(currentSelectedEntity);
	}

	void SceneEditorTab::OnScenePause(bool isPaused) {
		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(isPaused);
	}

	void SceneEditorTab::RenderContent()
	{
		// tint editor while playing
		int pushedStyleColorCount = 0;
		if (m_SceneState == SceneState::Play) 
		{
			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::dim);
			pushedStyleColorCount++;
		}

		if (m_ActiveScene == nullptr)
		{
			return;
		}


		m_SceneHierarchyPanel.OnImGuiRender();
		m_InspectorPanel.OnImGuiRender();
		if (m_Appearing) { ImGui::SetNextWindowFocus(); }

		bool isViewportOpen = false;

		/*Viewport*/ 
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::PanelTabPadding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_FirstUseEver);
			isViewportOpen = ImGui::Begin(m_ViewportPanelName.c_str(), nullptr, window_flags);


			auto viewport_min_region = ImGui::GetWindowContentRegionMin();
			auto viewport_max_region = ImGui::GetWindowContentRegionMax();
			auto viewport_offset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y };
			m_ViewportBounds[1] = { viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y };

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			m_ViewportPosition.x = m_ViewportBounds[0].x - viewport->Pos.x;
			m_ViewportPosition.y = m_ViewportBounds[0].y - viewport->Pos.y;

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = ImVec2(viewportSize.x, viewportSize.y);

			if (isViewportOpen)
			{
				ImGui::Image(*m_RenderTexture);
			}

			/* Drag drop target */ 
			{
				if (ImGui::BeginDragDropTarget()) 
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH", ImGuiDragDropFlags_AcceptBeforeDelivery))
					{
						std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));

						if (FileSystem::Exists(path) && (FileSystem::GetExtension(path) == ".escn")) 
						{
							// draw rect to show it can be draggable
							ImVec2 drawStart = ImVec2(m_ViewportBounds[0].x + 2, m_ViewportBounds[0].y + 2);
							ImVec2 drawEnd = ImVec2(m_ViewportBounds[1].x - 2, m_ViewportBounds[1].y - 2);
							ImGui::GetWindowDrawList()->AddRect(drawStart, drawEnd, IM_COL32(240, 240, 10, 240), 0.0f, ImDrawFlags_RoundCornersAll, 3.0f);

							if (payload->IsDelivery()) 
							{
								if (path.extension() == ".escn") 
								{
									if (m_EditorApplication) 
									{
										m_EditorApplication->RequestOpenResource(Project::GetEditorResourceManager()->GetRelativePath(path));
									}
								}
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}

			if (isViewportOpen) {
				ShowToolbarPlayPause();
				if (m_SceneState == SceneState::Edit or m_ActiveScene->IsPaused()) {
					m_ToolbarPanel.OnImGuiRender(m_ViewportBounds[0], m_ViewportBounds[1]);
				}
			}
			if (isViewportOpen) {
				ImGui::SetNextWindowPos({ m_ViewportBounds[0].x + 10, m_ViewportBounds[0].y + 40 });
				ImVec2 viewportSize = {
					m_ViewportBounds[1].x - m_ViewportBounds[0].x,
					m_ViewportBounds[1].y - m_ViewportBounds[0].y
				};
				//TODO: m_DebugInfoPanel.ShowDebugInfoPanel(m_Timestep, viewport_size);
			}

			ImGui::End();
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(pushedStyleColorCount);
		m_Appearing = false;
	}
	void SceneEditorTab::InitializeDockspace()
	{
		ImGuiID dock_id_1 = ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Left, 0.42f, NULL, &m_DockspaceID);
		ImGuiID dock_id_2 = ImGui::DockBuilderSplitNode(dock_id_1, ImGuiDir_Right, 0.52f, NULL, &dock_id_1);
		// 	ImGuiID dock_id_3 = ImGui::DockBuilderSplitNode(dock_id_1,     ImGuiDir_Down,  0.40f, NULL, &dock_id_1);
		ImGuiID dock_id_4 = ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Down, 0.32f, NULL, &m_DockspaceID);

		ImGui::DockBuilderDockWindow(m_ViewportPanelName.c_str(), m_DockspaceID);
		m_SceneHierarchyPanel.DockTo(dock_id_1);
		m_InspectorPanel.DockTo(dock_id_2);
		m_Appearing = true;
	}
	void SceneEditorTab::OnEvent(const std::optional<sf::Event>& evt)
	{
	}
	void SceneEditorTab::OnUpdate(TimeStep timestep)
	{
		m_TimeStep = timestep;

		if (m_ActiveScene == nullptr) {
			return;
		}

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f) 
		{
			m_RenderTexture->resize({ (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y });
		}

		switch (m_SceneState) {
		case SceneState::Edit:
			m_ActiveScene->OnUpdateEditor(timestep);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(timestep);
			break;
		}

		m_ToolbarPanel.OnUpdate();
	}
	void SceneEditorTab::SetPanelsContext()
	{
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_ToolbarPanel.SetContext(m_ActiveScene, &m_SceneHierarchyPanel);
		m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneHierarchyPanel);
		m_Appearing = true;
	}
	void SceneEditorTab::LoadScene(const std::filesystem::path& path)
	{
		if (!FileSystem::Exists(path))
			return;

		if (FileSystem::HasExtension(path) && FileSystem::GetExtension(path) != ".lscn")
			return;

		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
		}

		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer sceneSerializer = SceneSerializer(newScene);
		if (sceneSerializer.Deserialize(path))
		{
			m_EditorScene = newScene;
			m_ActiveScene = newScene;
			m_ActiveScenePath = path;
			
			SetPanelsContext();
		}
	}
	void SceneEditorTab::SaveScene()
	{
		if (m_SceneState == SceneState::Edit)
		{
			if (m_ActiveScenePath.empty())
			{
				std::filesystem::path filepath = FileSystem::SaveFileDialog({ { "Luden Scene (*.lscene)", "lscene" } });
				SceneSerializer sceneSerializer = SceneSerializer(m_ActiveScene);
				sceneSerializer.Serialize(m_ActiveScenePath);
			}
		}
	}
	void SceneEditorTab::ShowToolbarPlayPause()
	{
		constexpr ImVec2 toolbarSize = ImVec2(32.0f, 32.0f);
		const float padding = 4.0f;

		int visibleButtonCount = 1;

		if (m_SceneState == SceneState::Play) 
		{
			visibleButtonCount++;
		}

		if (m_ActiveScene->IsPaused()) 
		{
			visibleButtonCount++;
		}


		ImVec2 pos;
		pos.x = m_ViewportBounds[1].x - toolbarSize.x * visibleButtonCount - padding - padding;
		pos.y = m_ViewportBounds[0].y + padding;
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowBgAlpha(0.65f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, toolbarSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

		constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavInputs;

		if (not ImGui::Begin("##ToolbarPlayPause", nullptr, flags)) 
		{
			ImGui::End();
			ImGui::PopStyleVar(4);
			ImGui::PopStyleColor(3);
			return;
		}

		if (m_SceneState == SceneState::Play)
		{
			ImVec4 tint_color = ImVec4(1, 1, 1, 1);
			if (m_ActiveScene->IsPaused()) {
				tint_color = ImVec4(0.5f, 0.5f, 0.9f, 1.0f);
			}

			ResourceHandle pauseIconHandle = EditorResources::Pause;
			std::shared_ptr<Texture> pauseTexture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(pauseIconHandle));

			auto pauseTextureId = reinterpret_cast<char*>(static_cast<uintptr_t>(pauseTexture->GetTexture().getNativeHandle()));
			if (ImGui::ImageButton(pauseTextureId, pauseTexture->GetTexture(), sf::Vector2f(toolbarSize.x, toolbarSize.y), sf::Color::Transparent, sf::Color::White))
			{
				OnScenePause(!m_ActiveScene->IsPaused());
			}
			ImGui::SameLine();
		}

		ResourceHandle textureHandle = (m_SceneState == SceneState::Edit) ? EditorResources::Play : EditorResources::Stop;
		std::shared_ptr<Texture> texture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(textureHandle));
		auto textureId = reinterpret_cast<char*>(static_cast<uintptr_t>(texture->GetTexture().getNativeHandle()));

		if (ImGui::ImageButton(textureId, texture->GetTexture(), sf::Vector2f(toolbarSize.x, toolbarSize.y), sf::Color::Transparent, sf::Color::White))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}
		}


		ImGui::End();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
	}
}