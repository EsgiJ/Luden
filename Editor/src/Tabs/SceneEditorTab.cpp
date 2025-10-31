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
			std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
			SceneSerializer serializer = SceneSerializer(newScene);
			if (serializer.Deserialize(m_ActiveScenePath.string())) {
				m_ActiveScene = newScene;
				SetPanelsContext();
				m_ActiveScene->OnPhysics2DInit();
			}
		}


		m_SceneHierarchyPanel.SetSelectedEntityWithUUID(currentSelectedEntity);
		ImGui::SetWindowFocus(m_ViewportPanelName.c_str());
	}

	void SceneEditorTab::OnSceneStop()
	{
		OnScenePause(false);

		if (m_ActiveScene) 
		{
			m_ActiveScene->OnPhysics2DStop(); 
		}

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
		m_ResourceBrowserPanel.OnImGuiRender();

		if (m_Appearing)
		{ 
			ImGui::SetNextWindowFocus(); 
		}

		bool isViewportOpen = false;

		/*Viewport*/ 
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::PanelTabPadding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_FirstUseEver);
			isViewportOpen = ImGui::Begin(m_ViewportPanelName.c_str(), nullptr, window_flags);


			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			m_ViewportPosition.x = m_ViewportBounds[0].x - viewport->Pos.x;
			m_ViewportPosition.y = m_ViewportBounds[0].y - viewport->Pos.y;

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = ImVec2(viewportSize.x, viewportSize.y);

			if ((int)m_ViewportSize.x != (int)m_RenderTexture->getSize().x ||
				(int)m_ViewportSize.y != (int)m_RenderTexture->getSize().y) 
			{
				if (m_RenderTexture->resize({ (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y }))
				{
					//TODO: ASSERT unable to resize the render texture
				}
			}

			if (isViewportOpen)
			{
				ImGui::Image(*m_RenderTexture);       

				Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
				if (selectedEntity.IsValid())
				{
					ImDrawList* drawList = ImGui::GetWindowDrawList();

					DrawSelectedEntityOutline(drawList, selectedEntity);

					if (m_ToolbarPanel.GetSelectedTool() != ToolbarPanel::Tool::SELECT && m_SceneState == SceneState::Edit) 
					{
						DrawGizmo(drawList, selectedEntity, m_ToolbarPanel.GetSelectedTool());
					}
				}
			}

			// Draw Grid
			{
				if (m_ToolbarPanel.m_ShowGrid)
				{
					for (float x = m_ViewportBounds[0].x; x < m_ViewportBounds[1].x; x += (m_ToolbarPanel.m_GridStep))
						ImGui::GetWindowDrawList()->AddLine(ImVec2(x, m_ViewportBounds[0].y), ImVec2(x, m_ViewportBounds[1].y), IM_COL32(0, 0, 0, 100), 1.0f);

					for (float y = m_ViewportBounds[0].y; y < m_ViewportBounds[1].y; y += (m_ToolbarPanel.m_GridStep))
						ImGui::GetWindowDrawList()->AddLine(ImVec2(m_ViewportBounds[0].x, y), ImVec2(m_ViewportBounds[1].x, y), IM_COL32(0, 0, 0, 100), 1.0f);
				}
			}

			// Draw Collision
			{
				if (m_ToolbarPanel.m_ShowMovementCollision || m_ToolbarPanel.m_ShowVisionCollision)
				{
					if (m_ActiveScene != nullptr)
					{
						auto& entities = m_ActiveScene->GetEntityManager().GetEntities();

						for (auto& entity : entities)
						{
							if (entity.Has<BoxCollider2DComponent>() && entity.Has<TransformComponent>())
							{
								auto& transformComponent = entity.Get<TransformComponent>();
								glm::vec2 tempPos = WorldToScreen(transformComponent.pos);
								ImVec2 minPos = ImVec2(tempPos.x, tempPos.y);

								auto& boxComponent = entity.Get<BoxCollider2DComponent>();
								ImVec2 size = ImVec2(boxComponent.Size.x , boxComponent.Size.y);
								ImVec2 maxPos = ImVec2(minPos.x + size.x, minPos.y + size.y);

								if (m_ToolbarPanel.m_ShowMovementCollision)
								{
									ImGui::GetWindowDrawList()->AddRect(minPos, maxPos, IM_COL32(240, 240, 10, 240), 0.0f, ImDrawFlags_RoundCornersAll, 3.0f);
								}

								if (m_ToolbarPanel.m_ShowVisionCollision)
								{
									ImGui::GetWindowDrawList()->AddRect(minPos, maxPos, IM_COL32(100, 100, 10, 240), 0.0f, ImDrawFlags_RoundCornersAll, 3.0f);
								}
							}
						}
					}
				}
			}
			// Drag drop target
			{
				if (ImGui::BeginDragDropTarget()) 
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH"))
					{
						std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
						std::filesystem::path pathExtension = FileSystem::GetExtension(path);

						if (FileSystem::Exists(path))
						{
							// draw rect to show it can be draggable
							ImVec2 drawStart = ImVec2(m_ViewportBounds[0].x + 2, m_ViewportBounds[0].y + 2);
							ImVec2 drawEnd = ImVec2(m_ViewportBounds[1].x - 2, m_ViewportBounds[1].y - 2);
							ImGui::GetWindowDrawList()->AddRect(drawStart, drawEnd, IM_COL32(240, 240, 10, 240), 0.0f, ImDrawFlags_RoundCornersAll, 3.0f);

							if (payload->IsDelivery()) 
							{
								if (pathExtension == ".lscn")
								{
									if (m_EditorApplication) 
									{
										m_EditorApplication->RequestOpenResource(Project::GetEditorResourceManager()->GetRelativePath(path));
									}
								}

								if (pathExtension == ".png" || pathExtension == ".jpeg" || pathExtension == ".jpg")
								{
									if (m_EditorApplication)
									{
										auto newEntity = m_ActiveScene->CreateEntity(path.string());
										ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(path);
										auto texture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(handle));
										auto& textureComponent = newEntity.Add<TextureComponent>();
										textureComponent.textureHandle = handle;
										
										auto& transformComponent = newEntity.Add<TransformComponent>();
										SetEntityPositionToMouse(newEntity);
									}
								}
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}

			if (isViewportOpen) 
			{
				if (m_SceneState == SceneState::Edit || m_ActiveScene->IsPaused()) 
				{
					m_ToolbarPanel.OnImGuiRender(m_ViewportBounds[0], m_ViewportBounds[1]);
				}
				ShowToolbarPlayPause();

			}

			ImGui::End();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(pushedStyleColorCount);
		m_Appearing = false;
	}
	void SceneEditorTab::InitializeDockspace()
	{
		ImGuiID dockSpaceMainID = m_DockspaceID;

		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Left, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Right, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockDown = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Down, 0.25f, nullptr, &dockSpaceMainID);

		// Dock windows
		ImGui::DockBuilderDockWindow(m_ViewportPanelName.c_str(), dockSpaceMainID);
		m_SceneHierarchyPanel.DockTo(dockLeft);
		m_InspectorPanel.DockTo(dockRight);
		m_ResourceBrowserPanel.DockTo(dockDown);

		ImGui::DockBuilderFinish(dockSpaceMainID);
		m_Appearing = true;
	}
	void SceneEditorTab::OnEvent(const std::optional<sf::Event>& evt)
	{
		m_ToolbarPanel.OnEvent(evt);
	}
	void SceneEditorTab::OnUpdate(TimeStep timestep)
	{
		m_TimeStep = timestep;

		if (m_ActiveScene == nullptr) {
			return;
		}

		switch (m_SceneState) {
		case SceneState::Edit:
			m_ActiveScene->OnUpdateEditor(timestep, m_RenderTexture);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateEditor(timestep, m_RenderTexture);
			break;
		}

		m_ToolbarPanel.OnUpdate();
	}

	void SceneEditorTab::SetEntityPositionToMouse(Entity entity)
	{
		if (!entity.Has<TransformComponent>())
			return;

		ImVec2 mousePosImGui = ImGui::GetMousePos();
		ImVec2 viewportStart = m_ViewportBounds[0];

		int pixelX = (int)(mousePosImGui.x - viewportStart.x);
		int pixelY = (int)(mousePosImGui.y - viewportStart.y);

		int sfmlPixelY = pixelY;

		sf::Vector2i sfmlPixelPos(pixelX, sfmlPixelY);

		sf::Vector2f worldCoords = m_RenderTexture->mapPixelToCoords(sfmlPixelPos);

		auto& transform = entity.Get<TransformComponent>();

		transform.pos = { worldCoords.x, worldCoords.y };
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
			if (!m_ActiveScenePath.empty())
			{
				//std::filesystem::path filepath = FileSystem::SaveFileDialog({ { "Luden Scene (*.lscene)", "lscene" } });
				SceneSerializer sceneSerializer = SceneSerializer(m_ActiveScene);
				sceneSerializer.Serialize(m_ActiveScenePath);
			}
		}
	}
	void SceneEditorTab::ShowToolbarPlayPause()
	{
		constexpr ImVec2 toolbarSize = ImVec2(32.0f, 32.0f);
		const float padding = 4.0f;

		int visibleButtonCount = 2;

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

		if (!ImGui::Begin("##ToolbarPlayPause", nullptr, flags)) 
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

			std::shared_ptr<Texture> pauseTexture = EditorResources::PauseIcon;

			if (ImGui::ImageButton("PauseButton", pauseTexture->GetTexture(), sf::Vector2f(toolbarSize.x, toolbarSize.y), sf::Color::Transparent, sf::Color::White))
			{
				OnScenePause(!m_ActiveScene->IsPaused());
			}
			ImGui::SameLine();
		}

		std::shared_ptr<Texture> texture = (m_SceneState == SceneState::Edit) ? EditorResources::PlayIcon : EditorResources::StepIcon;

		if(ImGui::ImageButton("PlayButton", texture->GetTexture(), sf::Vector2f(toolbarSize.x, toolbarSize.y)))
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

	glm::vec2 SceneEditorTab::WorldToScreen(const glm::vec2& worldPos)
	{
		sf::Vector2f sfWorldPos(worldPos.x, worldPos.y);
		sf::Vector2i pixelPos = m_RenderTexture->mapCoordsToPixel(sfWorldPos);

		return { m_ViewportBounds[0].x + pixelPos.x,m_ViewportBounds[0].y + pixelPos.y };
	}

	void SceneEditorTab::DrawSelectedEntityOutline(ImDrawList* drawList, Entity entity)
	{
		if (!entity.IsValid() || !entity.Has<TransformComponent>() || !entity.Has<TextureComponent>())
			return;

		auto& transform = entity.Get<TransformComponent>();
		auto& textureComponent = entity.Get<TextureComponent>();
		auto texture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(textureComponent.textureHandle));

		if (!texture) return;

		sf::Vector2u texSize = texture->GetTexture().getSize();
		glm::vec2 scaledSize = { texSize.x * transform.scale.x, texSize.y * transform.scale.y };


		glm::vec2 worldCorners[] = {
			transform.pos,                                         
			transform.pos + glm::vec2(scaledSize.x, 0),           
			transform.pos + scaledSize,                            
			transform.pos + glm::vec2(0, scaledSize.y)            
		};

		ImVec2 screenCorners[4];
		for (int i = 0; i < 4; ++i)
		{
			glm::vec2 tempPos = WorldToScreen(worldCorners[i]);
			screenCorners[i] = ImVec2(tempPos.x, tempPos.y);
		}

		ImU32 color = IM_COL32(255, 255, 0, 255); 
		float thickness = 2.0f;

		drawList->AddLine(screenCorners[0], screenCorners[1], color, thickness); 
		drawList->AddLine(screenCorners[1], screenCorners[2], color, thickness); 
		drawList->AddLine(screenCorners[2], screenCorners[3], color, thickness); 
		drawList->AddLine(screenCorners[3], screenCorners[0], color, thickness); 
	}

	void SceneEditorTab::DrawGizmo(ImDrawList* drawList, Entity entity, ToolbarPanel::Tool tool)
	{
		if (!entity.IsValid() || !entity.Has<TransformComponent>())
			return;

		auto& transform = entity.Get<TransformComponent>();

		glm::vec2 centerWorldPos = transform.pos;
		if (entity.Has<TextureComponent>())
		{
			auto& textureComp = entity.Get<TextureComponent>();
			auto texture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(textureComp.textureHandle));
			if (texture)
			{
				centerWorldPos.x += texture->GetTexture().getSize().x * transform.scale.x / 2.0f;
				centerWorldPos.y += texture->GetTexture().getSize().y * transform.scale.y / 2.0f;
			}
		}
		glm::vec2 tempPos = WorldToScreen(centerWorldPos);
		ImVec2 center = ImVec2(tempPos.x, tempPos.y);

		const float axisLength = 50.0f;
		const float thickness = 3.0f;

		ImVec2 xAxisEnd = ImVec2(center.x + axisLength, center.y);
		ImVec2 yAxisEnd = ImVec2(center.x, center.y - axisLength); 

		ImU32 colorX = IM_COL32(255, 0, 0, 255);  
		ImU32 colorY = IM_COL32(0, 255, 0, 255);  
		ImU32 colorRot = IM_COL32(0, 150, 255, 255); 

		switch (tool)
		{
		case ToolbarPanel::Tool::MOVE:
		{
			drawList->AddLine(center, xAxisEnd, colorX, thickness);
			drawList->AddTriangleFilled(xAxisEnd, ImVec2(xAxisEnd.x - 10, xAxisEnd.y - 5), ImVec2(xAxisEnd.x - 10, xAxisEnd.y + 5), colorX);

			drawList->AddLine(center, yAxisEnd, colorY, thickness);
			drawList->AddTriangleFilled(yAxisEnd, ImVec2(yAxisEnd.x - 5, yAxisEnd.y + 10), ImVec2(yAxisEnd.x + 5, yAxisEnd.y + 10), colorY);

			drawList->AddRectFilled(ImVec2(center.x - 8, center.y - 8), ImVec2(center.x + 8, center.y + 8), IM_COL32(255, 255, 255, 100));
			break;
		}
		case ToolbarPanel::Tool::SCALE:
		{
			drawList->AddLine(center, xAxisEnd, colorX, thickness);
			drawList->AddRectFilled(ImVec2(xAxisEnd.x - 5, xAxisEnd.y - 5), ImVec2(xAxisEnd.x + 5, xAxisEnd.y + 5), colorX);

			drawList->AddLine(center, yAxisEnd, colorY, thickness);
			drawList->AddRectFilled(ImVec2(yAxisEnd.x - 5, yAxisEnd.y - 5), ImVec2(yAxisEnd.x + 5, yAxisEnd.y + 5), colorY);
			break;
		}
		case ToolbarPanel::Tool::ROTATE:
		{
			float radius = axisLength * 1.5f;

			drawList->AddCircle(center, radius, colorRot, 64, thickness);

			float rad = transform.angle * 3.1415926535f / 180.0f; 

			ImVec2 rotLineEnd = ImVec2(center.x + radius * std::cos(rad), center.y - radius * std::sin(rad));
			drawList->AddLine(center, rotLineEnd, IM_COL32(255, 255, 255, 255), 1.0f);

			drawList->AddLine(center, ImVec2(center.x + radius, center.y), IM_COL32(255, 255, 255, 50), 1.0f);
			break;
		}
		default:
			break;
		}
	}
}