#include "Panels/SceneHierarchyPanel.h"
#include "ECS/EntityManager.h"
#include "ECS/Entity.h"
#include "imgui.h"

#include <iostream>

#include <rttr/instance.h>
#include <rttr/type.h>
#include <rttr/variant.h>

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

		auto& mgr = Luden::EntityManager::Instance();
		rttr::instance inst = mgr;
		rttr::type t = rttr::type::get<Luden::EntityManager>();

		auto active_prop = t.get_property("m_Active");
		auto tags_prop = t.get_property("m_Tags");

		rttr::variant active_var = active_prop.get_value(inst);
		rttr::variant tags_var = tags_prop.get_value(inst);


		if (!active_prop.is_valid()) std::cerr << "m_Active not found\n";

		if (!active_var.is_valid()) std::cerr << "m_Active value invalid\n";

		auto active_view = active_var.create_sequential_view();
		auto tags_view = tags_var.create_sequential_view();

		const size_t N = tags_view.get_size();
		for (size_t i = 0; i < N; ++i)
		{
			bool alive = active_view.get_value(i).to_uint8() != 0;
			if (!alive)
				continue;

			//std::cout << "Tags type: " << tags_var.get_type().get_name().to_string() << "\n";
			//std::cout << "Is sequential: " << tags_var.get_type().is_sequential_container() << "\n";
			// tags are std::string
			auto tag_value = tags_view.get_value(i);
			if (!tag_value.is_valid() || !tag_value.is_type<std::string>()) {
				//std::cerr << "[SceneHierarchyPanel] Invalid tag at index " << i << "\n";
				continue;
			}

			std::string name = tag_value.get_value<std::string>();
			std::string label = "[" + std::to_string(i) + "] " + name;

			if (ImGui::Selectable(label.c_str(), false))
			{
				// TODO: handle selection of entity i
			}
		}

		ImGui::End();
	}
}
