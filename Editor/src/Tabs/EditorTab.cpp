#include "Tabs/EditorTab.h"
#include "Utils/EditorColors.h"

namespace Luden
{
	int EditorTab::s_TabIDCounter = 0;

	void EditorTab::OnImGuiRender()
	{
		if (BeginDockspace())
		{
			RenderContent();
			if (!m_DockspaceInitialized)
			{
				InitializeDockspace();
				m_DockspaceInitialized = true;
			}
		}
		ImGui::End();
	}

	void EditorTab::DockTo(ImGuiID dockspace_id) {
		ImGui::DockBuilderDockWindow(m_WindowName.c_str(), dockspace_id);
	}

	bool EditorTab::BeginDockspace()
	{
		if (m_Name.empty()) {
			return false;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::EditorTabPadding);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		if (m_IsDirty)
		{
			flags |= ImGuiWindowFlags_UnsavedDocument;
		}

		bool dockspace_open = ImGui::Begin(m_WindowName.c_str(), &m_IsOpen, flags);
		m_IsFocused = dockspace_open && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows | ImGuiFocusedFlags_DockHierarchy);

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
			ImGui::SetTooltip("%s", m_Name.c_str());
			ImGui::PopStyleVar(3);
		}

		ImGui::PopStyleVar(4);

		if (!dockspace_open)
			return false;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::PanelTabPadding);

		m_DockspaceID = ImGui::GetID(m_WindowName.c_str());

		if (!m_NoTabBar)
		{
			ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		}
		else
		{
			ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoTabBar);
		}

		ImGui::PopStyleVar();

		return true;
	}
}