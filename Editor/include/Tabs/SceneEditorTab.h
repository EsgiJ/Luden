#pragma once

#include "Tabs/EditorTab.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ToolbarPanel.h"
#include "Panels/ResourceBrowserPanel.h"
#include "Scene/Scene.h"

#include <SFML/Graphics/RenderTexture.hpp>

namespace Luden
{
	class SceneHierarchyPanel;
	class InspectorPanel;
	class ToolbarPanel;
	class ResourceBrowserPanel;

	class SceneEditorTab : public EditorTab
	{
	public:
		SceneEditorTab(const std::filesystem::path& name);
		virtual ~SceneEditorTab();

	public:
		virtual void OnScenePlay();
		virtual void OnSceneStop();
		void OnScenePause(bool isPaused = false);

		virtual void SaveScene();
	private:
		virtual void RenderContent() override final;
		virtual void InitializeDockspace() override final;

		virtual void OnEvent(const std::optional<sf::Event>& evt) override final;

		virtual void OnUpdate(TimeStep timestep) override final;

	private:
		void SetEntityPositionToMouse(Entity entity);
		void SetPanelsContext();

		virtual void LoadScene(const std::filesystem::path& path);

		void ShowToolbarPlayPause();

		ImVec2 WorldToScreen(const Math::Vec2& worldPos);
		void DrawSelectedEntityOutline(ImDrawList* drawList, Entity entity);
		void DrawGizmo(ImDrawList* drawList, Entity entity, ToolbarPanel::Tool tool);
	private:
		TimeStep m_TimeStep;

		std::shared_ptr<sf::RenderTexture> m_RenderTexture;

		std::shared_ptr<Scene> m_ActiveScene;
		std::shared_ptr<Scene> m_EditorScene;

		const std::string m_ViewportPanelName;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		ImVec2 m_ViewportSize{ 0.0f, 0.0f };
		ImVec2 m_ViewportBounds[2];


		ImVec2 m_ViewportPosition{ 0.0f, 0.0f };

		bool m_ShowColliders = true;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		InspectorPanel m_InspectorPanel;
		ToolbarPanel m_ToolbarPanel;
		ResourceBrowserPanel m_ResourceBrowserPanel;

		std::filesystem::path m_ActiveScenePath = std::filesystem::canonical(".");

		enum class SceneState 
		{
			Edit = 0,
			Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;

		bool m_ShowSelectionOutline = true;
		ImVec4 m_SelectionOutlineColor = ImVec4(1.0f, 0.44f, 0.1f, 0.84f);

		bool m_Appearing = true;
	};
}