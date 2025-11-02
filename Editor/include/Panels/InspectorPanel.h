#pragma once
	
#include "Panels/EditorPanel.h"
#include "Panels/SceneHierarchyPanel.h"

#include <functional>

namespace Luden
{
	class Scene;

	class InspectorPanel : public EditorPanel
	{
	public:
		InspectorPanel() : EditorPanel("Inspector") {}
		~InspectorPanel() = default;

		void SetContext(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* sceneHierarchyPanel);

	private:
		virtual void RenderContent() override final;

		void DrawEntityInInspector(Entity& entity);


		template <typename T>
		void DisplayComponentInInspector(const std::string& name, Entity& entity, const bool canDelete, const std::function<void()>& lambda);

		template <typename T>
		void DisplayComponentInPopup(const std::string& name);

		bool EntityButton(UUID& id);

	private:
		std::shared_ptr<Scene> m_Context;

		SceneHierarchyPanel* m_SceneHierarchyPanel = nullptr;
	};
}