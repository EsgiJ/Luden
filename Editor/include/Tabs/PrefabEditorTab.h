#pragma once
#include "Tabs/EditorTab.h"
#include "Scene/Prefab.h"
#include "Scene/Scene.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ResourceBrowserPanel.h"
#include "Render/Camera2D.h"

#include <SFML/Graphics.hpp>
#include <memory>

namespace Luden
{
	class EDITOR_API PrefabEditorTab : public EditorTab
	{
	public:
		PrefabEditorTab(const std::filesystem::path& filepath);
		~PrefabEditorTab();

		virtual void RenderContent() override;
		virtual void OnUpdate(TimeStep timestep) override;
		virtual void OnEvent(const std::optional<sf::Event>& evt) override;
		virtual void InitializeDockspace() override;

		void SavePrefab();
		void LoadPrefab(const std::filesystem::path& filepath);

	private:
		void RenderViewport();
		void RenderToolbar();
		void RenderAddEntityMenu();
		glm::vec2 WorldToScreen(const glm::vec3& worldPos);

	private:
		std::shared_ptr<Prefab> m_Prefab;
		std::shared_ptr<Scene> m_PrefabScene;
		std::filesystem::path m_FilePath;

		SceneHierarchyPanel m_HierarchyPanel;
		InspectorPanel m_InspectorPanel;
		ResourceBrowserPanel m_ResourceBrowserPanel;
		// Viewport
		std::shared_ptr<sf::RenderTexture> m_RenderTexture;
		Camera2D m_EditorCamera;
		ImVec2 m_ViewportSize = ImVec2(1280, 720);
		ImVec2 m_ViewportBounds[2];
		ImVec2 m_ViewportPosition;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		std::string m_ViewportPanelName;
		std::string m_ToolbarPanelName;
	};
}