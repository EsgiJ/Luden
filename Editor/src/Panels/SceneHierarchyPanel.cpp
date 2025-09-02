#include "Panels/SceneHierarchyPanel.h"
#include "Scene/Scene.h"
#include "ECS/Entity.h"
#include "Core/GameEngine.h"

#include "imgui.h"

#include <iostream>


namespace Luden::Editor
{
	void SceneHierarchyPanel::Render()
	{
		if (!m_Visible) return;
		if (!ImGui::Begin("Scene Hierarchy", &m_Visible))
		{
			ImGui::End();
			return;
		}

		auto entities = Luden::GameEngine::Get().GetCurrentScene()->GetEntityManager().GetEntities();

		for (auto& entity: entities)
		{
			std::string label = "[" + std::to_string(entity.UUID()) + "] " + entity.Tag();

			if (ImGui::Selectable(label.c_str(), false))
			{

			}
		}

		ImGui::End();
	}

}
