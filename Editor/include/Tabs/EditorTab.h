#pragma once

#include "Core/EditorApplication.h"
#include "Core/TimeStep.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <string>
#include <filesystem>

namespace Luden 
{

	class EditorApplication;

	class EditorTab 
	{
	public:
		explicit EditorTab(const std::filesystem::path& name)
			: m_TabID(++s_TabIDCounter)
			, m_Name(name.string())
			, m_WindowName(m_Name + "##tab" + std::to_string(m_TabID))
		{}
		virtual ~EditorTab() = default;

		void OnImGuiRender();
		void DockTo(ImGuiID dockspace_id);

		virtual void OnUpdate(TimeStep timestep) {}
		virtual void OnEvent(const std::optional<sf::Event>& evt) {}

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

		EditorApplication* m_EditorApplication;
		ImGuiID m_DockspaceID;
	private:
		static int s_TabIDCounter;
		bool m_DockspaceInitialized = false;
	};

}
