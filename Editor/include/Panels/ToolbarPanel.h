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

		void SetContext(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* sceneHierarchyPanel = nullptr);

		void InitValues(const std::shared_ptr<sf::RenderTexture> renderWindow, bool& isViewportHovered);

		void OnUpdate();
		void OnImGuiRender(const ImVec2& viewport_bound_min, const ImVec2& viewport_bound_max);

		void OnEvent(const std::optional<sf::Event>& evt);

		bool OnKeyPressed(const sf::Event::KeyPressed& key);
		bool OnMouseButtonPressed(const sf::Event::MouseButtonPressed& mouse);
		bool OnMouseButtonReleased(const sf::Event::MouseButtonReleased& mouse);
		bool OnMouseMoved(const sf::Event::MouseMoved& move);

	private:
		void ShowToolbar();

		void HandlePickEntityWithMouse();

		Math::Vec2 GetMouseDelta();

		void Moving();
		void Scaling();
		void Rotating();

	private:
		enum Tool 
		{
			SELECT = 0,
			MOVE,
			SCALE,
			ROTATE
		};
		Tool m_SelectedTool = Tool::SELECT;
		bool ToolImageButton(const ResourceHandle& textureHandle, Tool tool);

	private:
		std::shared_ptr<Scene> m_Context;
		SceneHierarchyPanel* m_SceneHierarchyPanel;
		std::shared_ptr<sf::RenderTexture> m_RenderWindow;

		constexpr static float m_Padding = 4.0f;
		constexpr static float m_ToolbarMinSize = 32.0f;


		ImVec2 m_ViewportBoundMin, m_ViewportBoundMax;

		bool* m_ViewportHovered = nullptr;

		Math::Vec2 m_MouseStart;
		bool m_ToolStart = false;
		bool m_ToolUsing = false;
	};

}