#include "Tabs/PrefabEditorTab.h"
#include "IO/FileSystem.h"
#include "Project/Project.h"
#include "Resource/ResourceImporter.h"
#include "Utils/EditorResources.h"

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui-SFML.h>

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
		ImGui::TextDisabled("|");
		ImGui::SameLine();

		ImGui::Text("Editing Prefab: %s", m_FilePath.filename().string().c_str());
	}

	void PrefabEditorTab::OnUpdate(TimeStep timestep)
	{
		if (m_ViewportHovered)
		{
			m_EditorCamera.Update();
		}
	}

	void PrefabEditorTab::OnEvent(const std::optional<sf::Event>& evt)
	{
		if (m_ViewportHovered)
		{
			m_EditorCamera.OnEvent(evt);
		}
	}

	void PrefabEditorTab::InitializeDockspace()
	{
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
			return;

		ResourceImporter::Serialize(m_Prefab);
		m_IsDirty = false;

		std::cout << "Prefab saved: " << m_FilePath << std::endl;
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
}