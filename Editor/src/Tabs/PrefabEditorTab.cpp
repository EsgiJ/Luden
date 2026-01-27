#include "Tabs/PrefabEditorTab.h"
#include "IO/FileSystem.h"
#include "Project/Project.h"
#include "Resource/ResourceImporter.h"
#include "Utils/EditorResources.h"

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Scene/SceneSerializer.h"
#include "Tabs/SceneEditorTab.h"
#include <glm/geometric.hpp>

namespace Luden
{
	PrefabEditorTab::PrefabEditorTab(const std::filesystem::path& filepath)
		: EditorTab(filepath)
		, m_ViewportPanelName("Viewport##" + std::to_string(m_TabID))
		, m_ToolbarPanelName("Toolbar##" + std::to_string(m_TabID))
	{
		m_RenderTexture = std::make_shared<sf::RenderTexture>();
		SetWindowName(filepath.filename().string());

		LoadPrefab(filepath);

		m_ResourceBrowserPanel.SetContext(m_EditorApplication);
		m_ToolbarPanel.SetContext(m_PrefabScene, &m_HierarchyPanel, &m_EditorCamera);
		m_ToolbarPanel.InitValues(m_RenderTexture, m_ViewportHovered);

		m_EditorCamera.SetDragEnabled(true);
		m_EditorCamera.SetZoomEnabled(true);
		m_EditorCamera.SetMinZoom(0.2f);
		m_EditorCamera.SetMaxZoom(5.0f);
		m_EditorCamera.SetPosition({ 0, 0 });
		m_EditorCamera.SetViewportSize({ m_ViewportSize.x, m_ViewportSize.y });
	}

	PrefabEditorTab::~PrefabEditorTab()
	{
	}

	void PrefabEditorTab::RenderContent()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_ToolbarPanelName.c_str()))
		{
			RenderToolbar();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		RenderViewport();

		m_HierarchyPanel.OnImGuiRender();
		m_ResourceBrowserPanel.OnImGuiRender();
		m_InspectorPanel.OnImGuiRender();
	}

	void PrefabEditorTab::RenderViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Begin(m_ViewportPanelName.c_str()))
		{
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = viewportSize;

			if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0)
			{
				m_EditorCamera.SetViewportSize({ m_ViewportSize.x, m_ViewportSize.y });

				if ((int)m_ViewportSize.x != (int)m_RenderTexture->getSize().x ||
					(int)m_ViewportSize.y != (int)m_RenderTexture->getSize().y)
				{
					m_RenderTexture->resize({ (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y });
				}

				if (m_PrefabScene)
				{
					m_PrefabScene->OnUpdateEditor(0.016f, m_RenderTexture, m_EditorCamera);
				}

				ImGui::Image(*m_RenderTexture);
			}

			// Draw Collision
			if (m_ToolbarPanel.m_ShowCollision)
			{
				if (m_PrefabScene != nullptr)
				{
					auto& entities = m_PrefabScene->GetEntityManager().GetEntities();
					for (auto& entity : entities)
					{
						if (entity.Has<BoxCollider2DComponent>() && entity.Has<TransformComponent>())
						{
							sf::Transform worldTransform = m_PrefabScene->GetWorldTransform(entity);
							auto& boxComponent = entity.Get<BoxCollider2DComponent>();

							float halfWidth = boxComponent.Size.x / 2.0f;
							float halfHeight = boxComponent.Size.y / 2.0f;

							sf::Vector2f corners[4] = {
								{boxComponent.Offset.x - halfWidth, boxComponent.Offset.y - halfHeight},
								{boxComponent.Offset.x + halfWidth, boxComponent.Offset.y - halfHeight},
								{boxComponent.Offset.x + halfWidth, boxComponent.Offset.y + halfHeight},
								{boxComponent.Offset.x - halfWidth, boxComponent.Offset.y + halfHeight}
							};

							ImVec2 screenCorners[4];
							for (int i = 0; i < 4; i++)
							{
								sf::Vector2f worldPos = worldTransform.transformPoint(corners[i]);
								glm::vec2 screenPos = WorldToScreen(glm::vec3(worldPos.x, worldPos.y, 0.0f));
								screenCorners[i] = ImVec2(screenPos.x, screenPos.y);
							}

							ImGui::GetWindowDrawList()->AddQuad(
								screenCorners[0],
								screenCorners[1],
								screenCorners[2],
								screenCorners[3],
								IM_COL32(240, 240, 10, 240),
								3.0f
							);

						}

						if (entity.Has<CircleCollider2DComponent>() && entity.Has<TransformComponent>())
						{
							sf::Transform worldTransform = m_PrefabScene->GetWorldTransform(entity);
							auto& circleComponent = entity.Get<CircleCollider2DComponent>();

							sf::Vector2f localCenter(circleComponent.Offset.x, circleComponent.Offset.y);
							sf::Vector2f worldCenter = worldTransform.transformPoint(localCenter);

							auto& transform = entity.Get<TransformComponent>();
							float worldScale = (transform.Scale.x + transform.Scale.y) / 2.0f;
							float worldRadius = circleComponent.Radius * worldScale;

							glm::vec2 screenCenter = WorldToScreen(glm::vec3(worldCenter.x, worldCenter.y, 0.0f));

							glm::vec2 screenEdge = WorldToScreen(glm::vec3(worldCenter.x + worldRadius, worldCenter.y, 0.0f));
							float screenRadius = glm::distance(screenCenter, screenEdge);

							ImGui::GetWindowDrawList()->AddCircle(
								ImVec2(screenCenter.x, screenCenter.y),
								screenRadius,
								IM_COL32(240, 240, 10, 240),
								32,
								3.0f
							);
						}
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void PrefabEditorTab::RenderToolbar()
	{
		if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save"))
		{
			SavePrefab();
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_ARROW_ROTATE_RIGHT " Reload"))
		{
			LoadPrefab(m_FilePath);
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_PLUS " Add Entity"))
		{
			ImGui::OpenPopup("AddEntityToPrefab");
		}

		if (ImGui::BeginPopup("AddEntityToPrefab"))
		{
			RenderAddEntityMenu();
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		ImGui::TextDisabled("|");
		ImGui::SameLine();

		ImGui::Text("Editing Prefab: %s", m_FilePath.filename().string().c_str());
	}

	void PrefabEditorTab::OnUpdate(TimeStep timestep)
	{
		if (m_ViewportHovered)
		{
			if (!m_Prefab)
			{
				Entity rootEntity = m_Prefab->GetRootEntity();
				if (rootEntity.IsValid())
				{
					glm::vec3 rootPosition = rootEntity.Get<TransformComponent>().Translation;
					m_EditorCamera.SetPosition(rootPosition);
				}
			}


			m_EditorCamera.Update(timestep);
			m_ToolbarPanel.OnUpdate();
		}
	}

	void PrefabEditorTab::OnEvent(const std::optional<sf::Event>& evt)
	{
		if (m_ViewportHovered)
		{
			m_EditorCamera.OnEvent(evt);
			m_ToolbarPanel.OnEvent(evt);
		}
	}

	void PrefabEditorTab::InitializeDockspace()
	{
		m_ResourceBrowserPanel.SetContext(m_EditorApplication);
		m_ToolbarPanel.SetContext(m_PrefabScene, &m_HierarchyPanel, &m_EditorCamera);
		m_ToolbarPanel.InitValues(m_RenderTexture, m_ViewportHovered);

		ImGuiID dockSpaceMainID = m_DockspaceID;

		ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Up, 0.08f, nullptr, &dockSpaceMainID);
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Left, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Right, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Down, 0.25f, nullptr, &dockSpaceMainID);

		ImGui::DockBuilderDockWindow(m_ToolbarPanelName.c_str(), dockTop);
		ImGui::DockBuilderDockWindow(m_ViewportPanelName.c_str(), dockSpaceMainID);
		m_HierarchyPanel.DockTo(dockLeft);
		m_InspectorPanel.DockTo(dockRight);
		m_ResourceBrowserPanel.DockTo(dockBottom);

		ImGui::DockBuilderFinish(dockSpaceMainID);
	}

	void PrefabEditorTab::SavePrefab()
	{
		if (!m_Prefab || !m_PrefabScene)
		{
			std::cerr << "[PrefabEditor] Cannot save: Invalid prefab or scene" << std::endl;
			return;
		}

		Entity rootEntity = m_Prefab->GetRootEntity();
		if (!rootEntity.IsValid())
			return;

		ResourceImporter::Serialize(m_Prefab);
		UpdateAllScenesWithPrefab(m_Prefab->Handle);
		m_IsDirty = false;
	}

	void PrefabEditorTab::LoadPrefab(const std::filesystem::path& filepath)
	{
		m_FilePath = filepath;

		if (FileSystem::Exists(filepath))
		{
			ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(filepath);
			m_Prefab = ResourceManager::GetResource<Prefab>(handle);

			if (m_Prefab)
			{
				m_PrefabScene = m_Prefab->GetScene();
				m_HierarchyPanel.SetContext(m_EditorApplication, m_PrefabScene);
				m_InspectorPanel.SetContext(m_PrefabScene, &m_HierarchyPanel, nullptr);
			}
		}
		else
		{
			m_Prefab = std::make_shared<Prefab>();
			m_PrefabScene = m_Prefab->GetScene();

			m_HierarchyPanel.SetContext(m_EditorApplication, m_PrefabScene);
			m_InspectorPanel.SetContext(m_PrefabScene, &m_HierarchyPanel, nullptr);
		}
	}

	glm::vec2 PrefabEditorTab::WorldToScreen(const glm::vec3& worldPos)
	{
		sf::Vector2f sfWorldPos(worldPos.x, worldPos.y);
		sf::Vector2i pixelPos = m_RenderTexture->mapCoordsToPixel(sfWorldPos, m_EditorCamera.GetView());
		return { m_ViewportBounds[0].x + pixelPos.x, m_ViewportBounds[0].y + pixelPos.y };
	}

	void PrefabEditorTab::RenderAddEntityMenu()
	{
		if (ImGui::MenuItem("Empty Entity"))
		{
			auto newEntity = m_PrefabScene->CreateEntity("New Entity");
			m_HierarchyPanel.SetSelectedEntity(newEntity);
		}

		ImGui::Separator();

		if (ImGui::BeginMenu(ICON_FA_CUBE " Add Prefab as Entity"))
		{
			auto resourceManager = Project::GetResourceManager();
			auto prefabHandles = resourceManager->GetAllResourcesWithType(ResourceType::Prefab);

			if (prefabHandles.empty())
			{
				ImGui::TextDisabled("No other prefabs available");
			}
			else
			{
				for (auto handle : prefabHandles)
				{
					if (handle == m_Prefab->Handle)
						continue;

					auto prefab = ResourceManager::GetResource<Prefab>(handle);
					if (!prefab)
						continue;

					const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
					std::string displayName = metadata.FilePath.filename().string();

					if (ImGui::MenuItem(displayName.c_str()))
					{
						glm::vec3 pos(0, 0, 0);
						Entity instance = m_PrefabScene->Instantiate(prefab, &pos, &pos, &pos);
						m_HierarchyPanel.SetSelectedEntity(instance);

						m_IsDirty = true;
					}
				}
			}

			ImGui::EndMenu();
		}
	}

	void PrefabEditorTab::UpdateAllScenesWithPrefab(ResourceHandle prefabHandle)
	{
		auto resourceManager = Project::GetEditorResourceManager();
		auto sceneHandles = resourceManager->GetAllResourcesWithType(ResourceType::Scene);

		int totalUpdated = 0;

		for (auto sceneHandle : sceneHandles)
		{
			int updated = UpdateSceneWithPrefab(sceneHandle, prefabHandle);
			totalUpdated += updated;
		}

		if (m_EditorApplication)
		{
			auto& tabs = m_EditorApplication->GetEditorTabs();

			for (auto& tab : tabs)
			{
				if (auto sceneTab = std::dynamic_pointer_cast<SceneEditorTab>(tab))
				{
					// Reload
					sceneTab->LoadScene(sceneTab->GetActiveScenePath());
				}
			}
		}
	}

	int PrefabEditorTab::UpdateSceneWithPrefab(ResourceHandle sceneHandle, ResourceHandle prefabHandle)
	{
		auto resourceManager = Project::GetEditorResourceManager();
		const auto& metadata = resourceManager->GetMetadata(sceneHandle);

		auto tempScene = std::make_shared<Scene>();
		SceneSerializer serializer(tempScene);

		std::filesystem::path scenePath = resourceManager->GetFileSystemPath(metadata);

		if (!serializer.Deserialize(scenePath.string()))
			return 0;
		

		auto prefab = ResourceManager::GetResource<Prefab>(prefabHandle);
		if (!prefab || !prefab->GetScene())
			return 0;

		Entity prefabRootEntity = prefab->GetRootEntity();
		if (!prefabRootEntity.IsValid())
			return 0;

		std::vector<Entity> instancesToUpdate;
		for (auto& entity : tempScene->GetEntityManager().GetEntities())
		{
			if (entity.Has<PrefabComponent>())
			{
				auto& prefabComp = entity.Get<PrefabComponent>();
				if (prefabComp.PrefabID == prefabHandle)
				{
					instancesToUpdate.push_back(entity);
				}
			}
		}

		if (instancesToUpdate.empty())
			return 0;

		for (auto& instance : instancesToUpdate)
		{
			UpdateEntityFromPrefab(tempScene, instance, prefabRootEntity);
		}

		serializer.Serialize(scenePath.string());

		return (int)instancesToUpdate.size();
	}

	void PrefabEditorTab::UpdateEntityFromPrefab(std::shared_ptr<Scene> scene,
		Entity instance,
		Entity prefabRootEntity)
	{
		PrefabComponent savedPrefabComp = instance.Get<PrefabComponent>();
		Entity savedParent = instance.GetParent();

		TransformComponent savedTransform = instance.Get<TransformComponent>();

		scene->CopyAllComponents(instance, prefabRootEntity, true);

		TransformComponent& instanceTransform = instance.Get<TransformComponent>();
		instanceTransform.Translation = savedTransform.Translation;
		instanceTransform.Scale = savedTransform.Scale;              
		instanceTransform.angle = savedTransform.angle;

		if (!instance.Has<PrefabComponent>())
		{
			instance.Add<PrefabComponent>();
		}
		instance.Get<PrefabComponent>() = savedPrefabComp;

		UpdateChildrenFromPrefab(scene, instance, prefabRootEntity);
	}

	void PrefabEditorTab::UpdateChildrenFromPrefab(std::shared_ptr<Scene> scene,
		Entity parentInstance,
		Entity prefabParent)
	{
		auto instanceChildren = parentInstance.Children();
		auto prefabChildren = prefabParent.Children();

		size_t minSize = std::min(instanceChildren.size(), prefabChildren.size());

		for (size_t i = 0; i < minSize; i++)
		{
			Entity childInstance = scene->TryGetEntityWithUUID(instanceChildren[i]);
			Entity prefabChild = prefabParent.GetScene()->TryGetEntityWithUUID(prefabChildren[i]);

			if (childInstance.IsValid() && prefabChild.IsValid())
			{
				scene->CopyAllComponents(childInstance, prefabChild, true);

				TransformComponent prefabChildTransform = prefabChild.Get<TransformComponent>();
				TransformComponent& instanceChildTransform = childInstance.Get<TransformComponent>();

				instanceChildTransform.Translation = prefabChildTransform.Translation;
				instanceChildTransform.Scale = prefabChildTransform.Scale;
				instanceChildTransform.angle = prefabChildTransform.angle;

				UpdateChildrenFromPrefab(scene, childInstance, prefabChild);
			}
		}
	}
}