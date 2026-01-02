#pragma once

#include "Panels/EditorPanel.h"
#include "Scene/Scene.h"
#include <Core/EditorApplication.h>
namespace Luden
{
	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel() : EditorPanel("Scene Hierarchy") {}
		~SceneHierarchyPanel() = default;

		void SetContext(EditorApplication* editorApplication, std::shared_ptr<Scene>& scene);
		bool IsSelectedEntityValid() const;

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		UUID GetSelectedEntityUUID();
		void SetSelectedEntity(const Entity& entity);
		void SetSelectedEntityWithUUID(const UUID& uuid);

		virtual void RenderContent() override final;

	private:
		void DrawEntityInSceneTree(Entity entity);
		void CreatePrefabFromEntity(Entity entity);
	private:
		EditorApplication* m_EditorApplication;
		std::shared_ptr<Scene> m_Context; 
		Entity m_SelectedEntity; 
		Entity m_EntityToConvertToPrefab;

		bool m_ShowCreatePrefabPopup = false;
		bool m_MouseReleased = false;	};
}