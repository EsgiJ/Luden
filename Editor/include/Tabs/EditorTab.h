#pragma once
#include <string>
#include <filesystem>
#include <imgui.h>
#include "imgui_internal.h"

namespace Luden {

	class EditorTab {
	public:
		explicit EditorTab(std::string name)
			: m_TabID(++s_TabIDCounter), 
			m_Name(name), 
			m_WindowName(m_Name + "##tab" + std::to_string(m_TabID))
		{}
		virtual ~EditorTab() = default;

		void OnImGuiRender();
		void DockTo(ImGuiID dockspace_id);

		virtual void OnUpdate(TimeStep timestep) {}

		const std::string& GetName() const { return m_Name; }
		const std::string& GetWindowName() const { return m_WindowName; }
		inline void SetWindowName(const std::string& name) { m_WindowName = name + "##tab" + std::to_string(m_TabID); }

		bool ShouldClose() const { return !m_IsOpen; }
		bool IsFocused() const { return m_IsFocused; }
		bool IsDirty() const { return m_IsDirty; }

		void MarkToBeClosed() { if (not m_IsDirty) { m_IsOpen = false; } }

	protected:
		bool BeginDockspace();
		virtual void RenderContent() {};
		virtual void InitializeDockspace() {};

	protected:
		int m_TabID;

		std::string m_Name;
		std::string m_WindowName;

		bool m_IsOpen = true;
		bool m_IsDirty = false;
		bool m_IsFocused = false;

		bool m_NoTabBar = false;

		ImGuiID m_DockspaceID;
	private:
		static inline int s_TabIDCounter = 0;
		bool m_DockspaceInitialized = false;
	};

}
