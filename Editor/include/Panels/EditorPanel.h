#pragma once

#include <imgui.h>

#include <string>

namespace Luden
{
	class EditorPanel
	{
	public:
		explicit EditorPanel(const std::string& name)
			: m_PanelID(s_PanelIDCounter++)
			, m_Name(name)
			, m_WindowName(m_Name + "##panel" + std::to_string(m_PanelID))
		{}

		void OnImGuiRender();
		void DockTo(ImGuiID dockspaceID);
	protected:

		int m_PanelID;
		const std::string m_Name;
		const std::string m_WindowName;

		bool BeginPanel();
		virtual void RenderContent() = 0;
	private:
		static int s_PanelIDCounter;
	};
}