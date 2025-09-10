#include "Panels/EditorPanel.h"

#include "Utils/EditorColors.h"

#include <imgui_internal.h>

namespace Luden
{
	void EditorPanel::OnImGuiRender()
	{
		if (BeginPanel())
		{
			RenderContent();
		}
		ImGui::End();
	}

	bool EditorPanel::BeginPanel()
	{
		ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f), ImGuiCond_FirstUseEver);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::PanelTabPadding);
		bool isOpen = ImGui::Begin(m_WindowName.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar);
		ImGui::PopStyleVar();

		if (!isOpen)
			return false;

		return true;
	}

	void EditorPanel::DockTo(ImGuiID dockspaceID)
	{
		ImGui::DockBuilderDockWindow(m_WindowName.c_str(), dockspaceID);
	}
}