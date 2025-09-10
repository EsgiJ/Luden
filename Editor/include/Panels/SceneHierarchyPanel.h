#pragma once

#include "Panels/EditorPanel.h"
#include "Scene/Scene.h"
namespace Luden
{
	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel() : EditorPanel("Scene Hierarchy") {}
		~SceneHierarchyPanel() = default;

		void SetContext(std::shared_ptr<Scene>& scene);
		bool IsSelectedEntityValid() const;

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		UUID GetSelectedEntityUUID();
		void SetSelectedEntity(const Entity& entity);
		void SetSelectedEntityWithUUID(const UUID& uuid);

		virtual void RenderContent() override final;

	private:
		void DrawEntityInSceneTree(Entity entity);

	private:
		std::shared_ptr<Scene> m_Context; 
		Entity m_SelectedEntity; 

		bool m_MouseReleased = false;
	};
}