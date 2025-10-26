#pragma once
#include "Scene/Scene.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Resource/Resource.h"

#include <imgui.h>
#include <SFML/Graphics/RenderTexture.hpp>

namespace Luden
{

	class ToolbarPanel 
	{
	public:
		ToolbarPanel() = default;
		ToolbarPanel(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* sceneHierarchyPanel = nullptr);
		~ToolbarPanel() = default;

		enum Tool
		{
			SELECT = 0,
			MOVE,
			SCALE,
			ROTATE
		};

		void SetContext(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* sceneHierarchyPanel = nullptr);

		void InitValues(const std::shared_ptr<sf::RenderTexture> renderWindow, bool& isViewportHovered);

		void OnUpdate();
		void OnImGuiRender(const ImVec2& viewport_bound_min, const ImVec2& viewport_bound_max);

		void OnEvent(const std::optional<sf::Event>& evt);

		bool OnKeyPressed(const sf::Event::KeyPressed& key);
		bool OnMouseButtonPressed(const sf::Event::MouseButtonPressed& mouse);
		bool OnMouseButtonReleased(const sf::Event::MouseButtonReleased& mouse);
		bool OnMouseMoved(const sf::Event::MouseMoved& move);

		Tool GetSelectedTool() { return m_SelectedTool; }
	private:
		void ShowToolbar();
		void HandlePickEntityWithMouse();

		Math::Vec2 GetMouseDelta();

		void Moving();
		void Scaling();
		void Rotating();

	private:
		Tool m_SelectedTool = Tool::SELECT;
		bool ToolImageButton(const std::shared_ptr<Texture>& texture, Tool tool);

	private:
		friend class SceneEditorTab;

		std::shared_ptr<Scene> m_Context;
		SceneHierarchyPanel* m_SceneHierarchyPanel;
		std::shared_ptr<sf::RenderTexture> m_RenderWindow;

		constexpr static float m_Padding = 4.0f;
		constexpr static float m_ToolbarMinSize = 32.0f;
		float m_GridStep = 64.0f;
		float m_MouseDeltaAccumX = 0.0f;
		float m_MouseDeltaAccumY = 0.0f;

		const float m_GridSizes[4] = { 16.0f, 32.0f, 64.0f, 128.0f };
		int m_SelectedGridIndex = 2;

		ImVec2 m_ViewportBoundMin, m_ViewportBoundMax;

		bool* m_ViewportHovered = nullptr;

		Math::Vec2 m_MouseStart;
		bool m_ToolStart = false;
		bool m_ToolUsing = false;
		bool m_ShowGrid = true;
		bool m_IsSnapEnabled = true;
	};

}