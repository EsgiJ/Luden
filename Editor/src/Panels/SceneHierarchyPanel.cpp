#include "Panels/SceneHierarchyPanel.h"

#include "ECS/EntityManager.h"

#include "imgui.h"

#include <rttr/type.h>
#include <rttr/variant.h>
#include <string>

namespace Luden::Editor
{
	void SceneHierarchyPanel::Render()
	{
		if (!ImGui::Begin("Scene Hierarchy"))
		{
			ImGui::End();
			return;
		}

		auto& mgr = Luden::EntityManager::Instance();
		rttr::instance em_inst = mgr;
		rttr::type em_type = rttr::type::get<Luden::EntityManager>();

		auto active_prop = em_type.get_property("m_Active");
		auto tags_prop = em_type.get_property("m_Tags");

		auto active_view = active_prop.get_value(em_inst).create_sequential_view();
		auto tags_view = tags_prop.get_value(em_inst).create_sequential_view();

		for (size_t i = 0; i < active_view.get_size(); ++i)
		{
			if (!active_view.get_value(i).to_bool())
				continue;

			rttr::variant var = tags_view.get_value(i);

			if (var.get_type() == rttr::type::get<std::string>())
			{
				std::string tag = var.get_value<std::string>();
				std::string label = "[" + std::to_string(i) + "] " + tag;

				if (ImGui::Selectable(label.c_str(), false))
				{
					// OnEntitySelected(i);
				}
			}
		}

		ImGui::End();
	}
}