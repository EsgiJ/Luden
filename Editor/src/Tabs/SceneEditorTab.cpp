#include "Tabs/SceneEditorTab.h"

#include "Utils/EditorColors.h"
#include "IO/FileSystem.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Project/Project.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include "glm/geometric.hpp"

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

		glm::vec2 initialPos = { m_ViewportSize.x * 0.5f, m_ViewportSize.y * 0.5f };

		m_EditorCamera.SetDragEnabled(true);
		m_EditorCamera.SetZoomEnabled(true);
		m_EditorCamera.SetMinZoom(0.2f);
		m_EditorCamera.SetMaxZoom(5.0f);
		m_EditorCamera.SetPosition(initialPos);
		m_EditorCamera.SetViewportSize({ m_ViewportSize.x, m_ViewportSize.y });
	}

	SceneEditorTab::~SceneEditorTab() 
	{
	}

	void SceneEditorTab::OnScenePlay()
	{
		OnScenePause(false);
		SaveScene();

		if (m_ActiveScenePath.empty())
			return;

		UUID currentSelectedEntity = m_SceneHierarchyPanel.GetSelectedEntityUUID();

		m_SceneState = SceneState::Play;

		if (!m_RenderTexture->resize({ PLAY_WIDTH, PLAY_HEIGHT }))
		{
			std::cerr << "[SceneEditorTab] Failed to resize render texture for play mode!" << std::endl;
		}

		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
		SceneSerializer serializer = SceneSerializer(newScene);
		if (serializer.Deserialize(m_ActiveScenePath.string()))
		{
			m_ActiveScene = newScene;
			SetPanelsContext();

			m_ActiveScene->SetViewportSize(PLAY_WIDTH, PLAY_HEIGHT);
			m_ActiveScene->OnRuntimeStart();
		}

		m_SceneHierarchyPanel.SetSelectedEntityWithUUID(currentSelectedEntity);
		ImGui::SetWindowFocus(m_ViewportPanelName.c_str());
	}

	void SceneEditorTab::OnSceneStop()
	{
		OnScenePause(false);

		if (m_ActiveScene) 
		{
			m_ActiveScene->OnRuntimeStop(); 
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

		LoadScene(m_ActiveScenePath);
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
		m_DebugSettingsPanel.OnImGuiRender();
		m_CollisionChannelPanel.OnImGuiRender();

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

			if (m_SceneState == SceneState::Edit)
			{
				if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
				{
					m_EditorCamera.SetViewportSize({ m_ViewportSize.x, m_ViewportSize.y });
				}

				if ((int)m_ViewportSize.x != (int)m_RenderTexture->getSize().x ||
					(int)m_ViewportSize.y != (int)m_RenderTexture->getSize().y)
				{
					if (m_RenderTexture->resize({ (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y }))
					{
						// Unable to resize
					}
				}
			}

			if (isViewportOpen)
			{
				if (m_SceneState == SceneState::Play)
				{
					float targetAspect = (float)PLAY_WIDTH / (float)PLAY_HEIGHT;
					float availableAspect = viewportSize.x / viewportSize.y;

					ImVec2 displaySize;
					if (availableAspect > targetAspect)
					{
						displaySize.y = viewportSize.y;
						displaySize.x = displaySize.y * targetAspect;
					}
					else
					{
						displaySize.x = viewportSize.x;
						displaySize.y = displaySize.x / targetAspect;
					}

					ImVec2 offset = ImVec2(
						(viewportSize.x - displaySize.x) * 0.5f,
						(viewportSize.y - displaySize.y) * 0.5f
					);

					ImVec2 cursorPos = ImGui::GetCursorPos();
					ImGui::SetCursorPos(ImVec2(cursorPos.x + offset.x, cursorPos.y + offset.y));

					ImVec2 imageMin = ImGui::GetCursorScreenPos();

					ImGui::Image(*m_RenderTexture, { displaySize.x, displaySize.y });

					m_ViewportBounds[0] = { imageMin.x, imageMin.y };
					m_ViewportBounds[1] = { imageMin.x + displaySize.x, imageMin.y + displaySize.y };
				}
				else
				{
					ImGui::Image(*m_RenderTexture);
				}

				// Draw Canvas Border 
				{
					float halfWidth = PLAY_WIDTH / 2.0f;   
					float halfHeight = PLAY_HEIGHT / 2.0f; 

					glm::vec3 topLeft(-halfWidth, -halfHeight, 0);
					glm::vec3 topRight(halfWidth, -halfHeight, 0);
					glm::vec3 bottomRight(halfWidth, halfHeight, 0);
					glm::vec3 bottomLeft(-halfWidth, halfHeight, 0);

					glm::vec2 screenTL = WorldToScreen(topLeft);
					glm::vec2 screenTR = WorldToScreen(topRight);
					glm::vec2 screenBR = WorldToScreen(bottomRight);
					glm::vec2 screenBL = WorldToScreen(bottomLeft);

					ImU32 canvasColor = IM_COL32(100, 255, 100, 255);  
					float canvasThickness = 2.0f;

					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(screenTL.x, screenTL.y),
						ImVec2(screenTR.x, screenTR.y),
						canvasColor, canvasThickness
					);
					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(screenTR.x, screenTR.y),
						ImVec2(screenBR.x, screenBR.y),
						canvasColor, canvasThickness
					);
					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(screenBR.x, screenBR.y),
						ImVec2(screenBL.x, screenBL.y),
						canvasColor, canvasThickness
					);
					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(screenBL.x, screenBL.y),
						ImVec2(screenTL.x, screenTL.y),
						canvasColor, canvasThickness
					);

					glm::vec2 screenCenter = WorldToScreen(glm::vec3(0, 0, 0));
					float crossSize = 20.0f;

					ImU32 centerColor = IM_COL32(255, 100, 100, 255);  // Red

					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(screenCenter.x - crossSize, screenCenter.y),
						ImVec2(screenCenter.x + crossSize, screenCenter.y),
						centerColor, 2.0f
					);
					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(screenCenter.x, screenCenter.y - crossSize),
						ImVec2(screenCenter.x, screenCenter.y + crossSize),
						centerColor, 2.0f
					);
				}

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

			if (m_ToolbarPanel.m_ShowGrid)
			{
				sf::Vector2f topLeft = m_RenderTexture->mapPixelToCoords(
					sf::Vector2i(0, 0),
					m_SceneState == SceneState::Edit ? m_EditorCamera.GetView() : m_RenderTexture->getView()
				);

				sf::Vector2f bottomRight = m_RenderTexture->mapPixelToCoords(
					sf::Vector2i(
						m_RenderTexture->getSize().x,  
						m_RenderTexture->getSize().y
					),
					m_SceneState == SceneState::Edit ? m_EditorCamera.GetView() : m_RenderTexture->getView()
				);

				float gridStep = m_ToolbarPanel.m_GridStep;

				float startX = floor(topLeft.x / gridStep) * gridStep;
				for (float x = startX; x <= bottomRight.x; x += gridStep)
				{
					glm::vec2 p1 = WorldToScreen(glm::vec3(x, topLeft.y, 0));
					glm::vec2 p2 = WorldToScreen(glm::vec3(x, bottomRight.y, 0));
					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(p1.x, p1.y),
						ImVec2(p2.x, p2.y),
						IM_COL32(255, 255, 255, 100),
						1.0f
					);
				}

				float startY = floor(topLeft.y / gridStep) * gridStep;
				for (float y = startY; y <= bottomRight.y; y += gridStep)
				{
					glm::vec2 p1 = WorldToScreen(glm::vec3(topLeft.x, y, 0));
					glm::vec2 p2 = WorldToScreen(glm::vec3(bottomRight.x, y, 0));
					ImGui::GetWindowDrawList()->AddLine(
						ImVec2(p1.x, p1.y),
						ImVec2(p2.x, p2.y),
						IM_COL32(255, 255, 255, 100),
						1.0f
					);
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
								if (pathExtension == ".lscene")
								{
									if (m_EditorApplication) 
									{
										m_EditorApplication->RequestOpenResource(Project::GetEditorResourceManager()->GetRelativePath(path));
									}
								}

								if (pathExtension == ".lsprite")
								{
									if (m_EditorApplication)
									{
										auto newEntity = m_ActiveScene->CreateEntity(path.string());
										ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(path);
										auto sprite = ResourceManager::GetResource<Sprite>(handle);
										auto& spriteRendererComponent = newEntity.Add<SpriteRendererComponent>();
										spriteRendererComponent.spriteHandle = handle;
										
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
		SetPanelsContext();
		ImGuiID dockSpaceMainID = m_DockspaceID;

		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Left, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Right, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockDown = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Down, 0.25f, nullptr, &dockSpaceMainID);

		// Dock windows
		ImGui::DockBuilderDockWindow(m_ViewportPanelName.c_str(), dockSpaceMainID);
		m_SceneHierarchyPanel.DockTo(dockLeft);
		m_InspectorPanel.DockTo(dockRight);
		m_ResourceBrowserPanel.DockTo(dockDown);
		m_DebugSettingsPanel.DockTo(dockRight);
		m_CollisionChannelPanel.DockTo(dockLeft);

		ImGui::DockBuilderFinish(dockSpaceMainID);
		m_Appearing = true;
	}

	void SceneEditorTab::OnEvent(const std::optional<sf::Event>& evt)
	{
		m_ToolbarPanel.OnEvent(evt);

		if (m_ActiveScene)
		{
			if (m_SceneState == SceneState::Edit && m_ViewportHovered)
			{
				m_EditorCamera.OnEvent(evt);
			}
			m_ActiveScene->OnEvent(evt);
		}
	}

	void SceneEditorTab::OnUpdate(TimeStep timestep)
	{
		m_TimeStep = timestep;

		if (m_ActiveScene == nullptr) {
			return;
		}

		switch (m_SceneState) {
		case SceneState::Edit:
			m_ActiveScene->OnUpdateEditor(timestep, m_RenderTexture, m_EditorCamera);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(timestep, m_RenderTexture);
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

		sf::Vector2i sfmlPixelPos(pixelX, pixelY);

		sf::Vector2f worldCoords;

		if (m_SceneState == SceneState::Edit)
		{
			worldCoords = m_RenderTexture->mapPixelToCoords(sfmlPixelPos, m_EditorCamera.GetView());
		}
		else if (m_SceneState == SceneState::Play)
		{
			Entity cameraEntity = m_ActiveScene->GetMainCameraEntity();
			if (cameraEntity.IsValid())
			{
				const sf::View& cameraView = cameraEntity.Get<Camera2DComponent>().Camera.GetView();
				worldCoords = m_RenderTexture->mapPixelToCoords(sfmlPixelPos, cameraView);
			}
			else
			{
				worldCoords = m_RenderTexture->mapPixelToCoords(sfmlPixelPos);
			}
		}

		auto& transform = entity.Get<TransformComponent>();
		transform.Translation = { worldCoords.x, worldCoords.y, 0.0f };
	}

	void SceneEditorTab::SetPanelsContext()
	{
		m_SceneHierarchyPanel.SetContext(m_EditorApplication, m_ActiveScene);
		m_ToolbarPanel.SetContext(m_ActiveScene, &m_SceneHierarchyPanel, &m_EditorCamera);
		m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneHierarchyPanel, m_EditorApplication);
		m_ResourceBrowserPanel.SetContext(m_EditorApplication);
		m_Appearing = true;
	}

	void SceneEditorTab::LoadScene(const std::filesystem::path& path)
	{
		if (!FileSystem::Exists(path))
			return;

		if (FileSystem::HasExtension(path) && FileSystem::GetExtension(path) != ".lscene")
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

	glm::vec2 SceneEditorTab::WorldToScreen(const glm::vec3& worldPos)
	{
		sf::Vector2f sfWorldPos(worldPos.x, worldPos.y);

		const sf::View* activeView = nullptr;

		if (m_SceneState == SceneState::Edit)
		{
			activeView = &m_EditorCamera.GetView();
		}
		else if (m_SceneState == SceneState::Play)
		{
			Entity cameraEntity = m_ActiveScene->GetMainCameraEntity();
			if (cameraEntity.IsValid())
			{
				activeView = &cameraEntity.Get<Camera2DComponent>().Camera.GetView();
			}
		}

		if (!activeView)
			return { worldPos.x, worldPos.y };

		sf::Vector2i pixelPos = m_RenderTexture->mapCoordsToPixel(sfWorldPos, *activeView);

		if (m_SceneState == SceneState::Play)
		{
			float displayWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
			float displayHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

			float scaleX = displayWidth / (float)PLAY_WIDTH;
			float scaleY = displayHeight / (float)PLAY_HEIGHT;

			return {
				m_ViewportBounds[0].x + pixelPos.x * scaleX,
				m_ViewportBounds[0].y + pixelPos.y * scaleY
			};
		}

		return { m_ViewportBounds[0].x + pixelPos.x, m_ViewportBounds[0].y + pixelPos.y };
	}

	void SceneEditorTab::DrawSelectedEntityOutline(ImDrawList* drawList, Entity entity)
	{
		if (!entity.IsValid() || !entity.Has<TransformComponent>() || !entity.Has<SpriteRendererComponent>())
			return;

		auto& spriteRendererComponent = entity.Get<SpriteRendererComponent>();
		auto sprite = ResourceManager::GetResource<Sprite>(spriteRendererComponent.spriteHandle);
		if (!sprite) return;

		sf::Transform worldTransform = m_ActiveScene->GetWorldTransform(entity);

		sf::Vector2u texSize = sprite->GetSize();
		glm::vec2 pivot = sprite->GetPivot();

		float halfWidth = texSize.x / 2.0f;
		float halfHeight = texSize.y / 2.0f;

		sf::Vector2f localCorners[4] = {
			{-halfWidth, -halfHeight}, 
			{ halfWidth, -halfHeight}, 
			{ halfWidth,  halfHeight}, 
			{-halfWidth,  halfHeight}  
		};

		float pivotOffsetX = texSize.x * pivot.x;
		float pivotOffsetY = texSize.y * pivot.y;

		for (int i = 0; i < 4; i++)
		{
			localCorners[i].x -= pivotOffsetX - halfWidth;
			localCorners[i].y -= pivotOffsetY - halfHeight;
		}

		ImVec2 screenCorners[4];
		for (int i = 0; i < 4; i++)
		{
			sf::Vector2f worldPos = worldTransform.transformPoint(localCorners[i]);
			glm::vec2 screenPos = WorldToScreen(glm::vec3(worldPos.x, worldPos.y, 0.0f));
			screenCorners[i] = ImVec2(screenPos.x, screenPos.y);
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

		sf::Transform worldTransform = m_ActiveScene->GetWorldTransform(entity);

		sf::Vector2f worldCenter = worldTransform.transformPoint({ 0.0f, 0.0f });
		glm::vec2 screenCenter = WorldToScreen(glm::vec3(worldCenter.x, worldCenter.y, 0.0f));
		ImVec2 center = ImVec2(screenCenter.x, screenCenter.y);

		sf::Vector2f worldRight = worldTransform.transformPoint({ 1.0f, 0.0f });
		sf::Vector2f worldOrigin = worldTransform.transformPoint({ 0.0f, 0.0f });
		float worldAngle = std::atan2(
			worldOrigin.y - worldRight.y,  
			worldRight.x - worldOrigin.x
		);

		const float axisLength = 50.0f;
		const float thickness = 3.0f;

		float cosAngle = std::cos(worldAngle);
		float sinAngle = std::sin(worldAngle);

		ImVec2 xAxisEnd = ImVec2(
			center.x + axisLength * cosAngle,
			center.y - axisLength * sinAngle 
		);

		ImVec2 yAxisEnd = ImVec2(
			center.x - axisLength * sinAngle,
			center.y - axisLength * cosAngle
		);

		ImU32 colorX = IM_COL32(255, 0, 0, 255);
		ImU32 colorY = IM_COL32(0, 255, 0, 255);
		ImU32 colorRot = IM_COL32(0, 150, 255, 255);

		switch (tool)
		{
		case ToolbarPanel::Tool::MOVE:
		{
			drawList->AddLine(center, xAxisEnd, colorX, thickness);

			ImVec2 xArrowDir = ImVec2(cosAngle, -sinAngle);
			ImVec2 xArrowPerp = ImVec2(-sinAngle, -cosAngle);

			drawList->AddTriangleFilled(
				xAxisEnd,
				ImVec2(xAxisEnd.x - 10 * xArrowDir.x - 5 * xArrowPerp.x,
					xAxisEnd.y - 10 * xArrowDir.y - 5 * xArrowPerp.y),
				ImVec2(xAxisEnd.x - 10 * xArrowDir.x + 5 * xArrowPerp.x,
					xAxisEnd.y - 10 * xArrowDir.y + 5 * xArrowPerp.y),
				colorX
			);

			drawList->AddLine(center, yAxisEnd, colorY, thickness);

			ImVec2 yArrowDir = ImVec2(-sinAngle, -cosAngle);
			ImVec2 yArrowPerp = ImVec2(-cosAngle, sinAngle);

			drawList->AddTriangleFilled(
				yAxisEnd,
				ImVec2(yAxisEnd.x - 10 * yArrowDir.x - 5 * yArrowPerp.x,
					yAxisEnd.y - 10 * yArrowDir.y - 5 * yArrowPerp.y),
				ImVec2(yAxisEnd.x - 10 * yArrowDir.x + 5 * yArrowPerp.x,
					yAxisEnd.y - 10 * yArrowDir.y + 5 * yArrowPerp.y),
				colorY
			);

			drawList->AddRectFilled(
				ImVec2(center.x - 8, center.y - 8),
				ImVec2(center.x + 8, center.y + 8),
				IM_COL32(255, 255, 255, 100)
			);
			break;
		}

		case ToolbarPanel::Tool::SCALE:
		{
			drawList->AddLine(center, xAxisEnd, colorX, thickness);

			ImVec2 xDir = ImVec2(cosAngle, -sinAngle);
			ImVec2 xPerp = ImVec2(-sinAngle, -cosAngle);

			drawList->AddQuadFilled(
				ImVec2(xAxisEnd.x - 5 * xDir.x - 5 * xPerp.x, xAxisEnd.y - 5 * xDir.y - 5 * xPerp.y),
				ImVec2(xAxisEnd.x + 5 * xDir.x - 5 * xPerp.x, xAxisEnd.y + 5 * xDir.y - 5 * xPerp.y),
				ImVec2(xAxisEnd.x + 5 * xDir.x + 5 * xPerp.x, xAxisEnd.y + 5 * xDir.y + 5 * xPerp.y),
				ImVec2(xAxisEnd.x - 5 * xDir.x + 5 * xPerp.x, xAxisEnd.y - 5 * xDir.y + 5 * xPerp.y),
				colorX
			);

			drawList->AddLine(center, yAxisEnd, colorY, thickness);

			ImVec2 yDir = ImVec2(-sinAngle, -cosAngle);
			ImVec2 yPerp = ImVec2(-cosAngle, sinAngle);

			drawList->AddQuadFilled(
				ImVec2(yAxisEnd.x - 5 * yDir.x - 5 * yPerp.x, yAxisEnd.y - 5 * yDir.y - 5 * yPerp.y),
				ImVec2(yAxisEnd.x + 5 * yDir.x - 5 * yPerp.x, yAxisEnd.y + 5 * yDir.y - 5 * yPerp.y),
				ImVec2(yAxisEnd.x + 5 * yDir.x + 5 * yPerp.x, yAxisEnd.y + 5 * yDir.y + 5 * yPerp.y),
				ImVec2(yAxisEnd.x - 5 * yDir.x + 5 * yPerp.x, yAxisEnd.y - 5 * yDir.y + 5 * yPerp.y),
				colorY
			);
			break;
		}

		case ToolbarPanel::Tool::ROTATE:
		{
			float radius = axisLength * 1.5f;

			drawList->AddCircle(center, radius, colorRot, 64, thickness);

			ImVec2 rotLineEnd = ImVec2(
				center.x + radius * cosAngle,
				center.y - radius * sinAngle
			);
			drawList->AddLine(center, rotLineEnd, IM_COL32(255, 255, 255, 255), 2.0f);

			drawList->AddLine(
				center,
				ImVec2(center.x + radius, center.y),
				IM_COL32(255, 255, 255, 50),
				1.0f
			);
			break;
		}

		default:
			break;
		}
	}
}